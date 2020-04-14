#include <iostream>
#include <iomanip>
#include <fstream>

#include <algorithm>
#include <vector>
#include <map>
#include <deque>

using namespace std;

struct mmBlock
{
    int tag = 0;
    int index = 0;
};

struct cacheBlock
{
    int dirtyBit = 0;
    int validBit = 0;
    int tag = 0;
    string data;
    deque<int> fifoQueue; //first block in a cache set will contain the queue for the whole set
    deque<int> lruQueue;
};

struct memoryReference
{
    int memoryAddress = 0;
    int mmBlock = 0;
    int cmSet = 0;
    string cmBlock;
    string hitMiss;
};

int countBits(int block_size)
{
    //counts the bits needed to represent an integer
    int count = 0;
    while (block_size)
    {
        count++;
        block_size = block_size >> 1;
    }
    return count;
}

string toBinary(int n, int tagBits)
{
    string binary;
    int count = 0;
    for (int i = 0; n > 0; i++)
    {
        binary.insert(0, to_string(n % 2));
        n = n / 2;
        count++;
    }
    for (int i = 0; i < tagBits - count; i++)
    {
        binary.insert(0, "0");
    }
    return binary;
}

vector<int> possibleBlocks(int index, int setDegree)
{
    //returns possible cache blocks given a cache set and the set degree
    vector<int> possibleBlocks;
    for (int i = 0; i < setDegree; i++)
    {
        possibleBlocks.push_back((index * setDegree) + i);
    }
    return possibleBlocks;
}

void clearCacheBlock(int cmBlock, map<int, cacheBlock> &cacheTable)
{
    //clears a given cache block in the cache table
    cacheTable[cmBlock].validBit = 0;
    cacheTable[cmBlock].dirtyBit = 0;
    cacheTable[cmBlock].tag = 0;
    cacheTable[cmBlock].data.erase();
}

void updateCacheTable(int cmBlock, int memoryBlock, int tag, map<int, cacheBlock> &cacheTable)
{
    //given a cache block, memory block, tag, and the cache table, updates the cache table with a new value
    cacheTable[cmBlock].validBit = 1;
    cacheTable[cmBlock].tag = tag;
    cacheTable[cmBlock].data.append("mm blk # ");
    cacheTable[cmBlock].data.append(to_string(memoryBlock));
}
void printHitRate(map<int, memoryReference> referenceTable)
{
    double bestHitRate = 0;
    double actualHitRate = 0;
    vector<int> mmBlocks;

    for (int i = 0; i < referenceTable.size(); i++)
    {
        for (int j = 0; j < mmBlocks.size(); j++)
        {
            if (referenceTable[i].mmBlock == mmBlocks.at(j))
            {
                bestHitRate++;
                break;
            }
        }
        mmBlocks.push_back(referenceTable[i].mmBlock);
        if (referenceTable[i].hitMiss == "hit")
            actualHitRate++;
    }
    cout << "Highest possible hit rate = ";
    cout << bestHitRate << "/" << referenceTable.size() << " = ";
    cout << (bestHitRate / (double)referenceTable.size()) * 100 << "%" << endl;
    cout << "Actual hit rate = ";
    cout << actualHitRate << "/" << referenceTable.size() << " = ";
    cout << (actualHitRate / (double)referenceTable.size()) * 100 << "%" << endl;
    cout << endl;
}
void printReferenceTable(int setDegree, map<int, memoryReference> referenceTable)
{
    cout << "main memory address   "
         << "mm blk #   "
         << "cm set #   "
         << left << setw(setDegree + 11) << "cm blk #   "
         << "hit/miss" << endl;
    cout << "_______________________________________________________________\n";
    for (int i = 0; i < referenceTable.size(); i++)
    {
        cout << left << setw(22) << referenceTable[i].memoryAddress;
        cout << left << setw(11) << referenceTable[i].mmBlock;
        cout << left << setw(11) << referenceTable[i].cmSet;
        cout << left << setw(setDegree + 11) << referenceTable[i].cmBlock;
        cout << referenceTable[i].hitMiss << endl;
    }
    cout << endl;
}

void printCacheTable(int tagSize, map<int, cacheBlock> cacheTable)
{
    cout << "Cache blk #   "
         << "dirty bit   "
         << "valid bit   "
         << left << setw(3 + tagSize) << "tag   "
         << "Data" << endl;
    cout << "________________________________________________\n";
    for (int i = 0; i < cacheTable.size(); i++)
    {
        cout << left << setw(14) << i;
        cout << left << setw(12) << cacheTable[i].dirtyBit;
        cout << left << setw(12) << cacheTable[i].validBit;
        if (cacheTable[i].validBit == 1)
        {
            cout << left << setw(3 + tagSize) << toBinary(cacheTable[i].tag, tagSize);
            cout << left << setw(11) << cacheTable[i].data << endl;
        }
        else
        {
            string tagOutput;
            for (int i = 0; i < tagSize; i++)
                tagOutput.append("x");
            cout << left << setw(3 + tagSize) << tagOutput;
            cout << left << setw(11) << "xxx" << endl;
        }
    }
    cout << endl;
}
int main()
{
    string repeatProgram = "y";
    while (repeatProgram == "y")
    {
        int mmSize;
        cout << "Enter the size of main memory in bytes:";
        cin >> mmSize;

        int cacheSize;
        cout << "Enter the size of the cache in bytes:";
        cin >> cacheSize;

        int blockSize;
        cout << "Enter the cache block/line size:";
        cin >> blockSize;
        cout << endl;

        int setDegree;
        cout << "Enter the degree of set-associativity (input n for an n-way set-associative mapping):";
        cin >> setDegree;

        string replacement_policy;
        cout << "Enter the replacement policy (L = LRU, F = FIFO):";
        cin >> replacement_policy;

        string inputFile;
        cout << "Enter the name of the input file containing the list of memory references generated by the CPU:";
        cin >> inputFile;

        int mmBlocks = mmSize / blockSize;       //max amount of memory blocks
        int cacheBlocks = cacheSize / blockSize; //max amount of cache blocks
        int cacheSets = cacheBlocks / setDegree; //max amount of cache sets

        int offsetBits = countBits(blockSize - 1);           //# of bits for offset
        int indexBits = countBits(cacheSets - 1);            //# of bits for the index ()
        int tagBits = countBits((mmBlocks / cacheSets) - 1); //#of bits for the tag()

        int validBytes = cacheBlocks / 8;                             //# of bytes of valid bit data
        int tagBytes = (tagBits * cacheBlocks) / 8;                   //# of bytes of tag data
        int totalCacheSize = cacheSize + (2 * validBytes) + tagBytes; //total cache size in bytes(includes dirty bytes)
        int addressLines = indexBits + tagBits + offsetBits;          //address lines required (tag + index + offset)

        //create a blank cache table(all values are 0)
        map<int, cacheBlock> cacheTable;
        for (int i = 0; i < cacheBlocks; i++)
        {
            cacheBlock newBlock;
            cacheTable[i] = newBlock;
        }

        //create the main memory table(sets up tag, index, offset and block number)
        map<int, mmBlock> mmTable;
        int tag = 0;
        int index = 0;
        for (int i = 0; i < mmBlocks; i++)
        {
            if (i % cacheSets == 0 && i != 0)
            {
                tag++;
                index = 0;
            }
            mmBlock newBlock;
            newBlock.tag = tag;
            newBlock.index = index;
            mmTable[i] = newBlock;
            cout << i << " " << newBlock.tag << " " << newBlock.index << " " << endl;
            index++;
        }

        ifstream inFile(inputFile);
        string file_text;
        map<int, memoryReference> referenceTable; //create a table of references

        //dont really care about the first two lines
        getline(inFile, file_text);
        getline(inFile, file_text);

        //main loop, goes through each instruction in the text file given
        for (int i = 0; getline(inFile, file_text); i++)
        {
            //first char is read or write, second char is the memory address to read/write
            char instruction = file_text[0];
            file_text.erase(0, 1); //get rid of instruction and space characters
            int memoryAddress = stoi(file_text);
            int memoryBlock = memoryAddress / blockSize;

            memoryReference newReference;
            newReference.memoryAddress = memoryAddress;      //add memory address to referenceTable
            newReference.mmBlock = memoryBlock;              //add memory block to referenceTable
            newReference.cmSet = mmTable[memoryBlock].index; //add cache set mm block belongs to to referenceTable

            vector<int> cacheBlocks = possibleBlocks(mmTable[memoryBlock].index, setDegree);
            for (int i = 0; i < cacheBlocks.size(); i++)
            {
                newReference.cmBlock.append(to_string(cacheBlocks[i])); //add cache blocks mm block belongs to
                if (i != cacheBlocks.size() - 1)
                {
                    newReference.cmBlock.append(",");
                }
            }

            //check if the memory address was already in the cache(add hit or miss to referenceTable)
            for (int i = 0; i < cacheBlocks.size(); i++)
            {
                if (cacheTable[cacheBlocks[i]].tag == mmTable[memoryBlock].tag && cacheTable[cacheBlocks[i]].validBit == 1)
                {
                    if (instruction == 'W')
                    {
                        cacheTable[cacheBlocks[i]].dirtyBit = 1;
                    }
                    newReference.hitMiss = "hit";
                    break; // the value is already in the cache so no reason to continue
                }
                else
                {
                    newReference.hitMiss = "miss";
                }
            }
            if (newReference.hitMiss == "miss")
            {
                bool replacement = false;
                for (int i = 0; i < cacheBlocks.size(); i++)
                {
                    if (cacheTable[cacheBlocks[i]].validBit == 0)
                    {
                        if (instruction == 'W')
                        {
                            cacheTable[cacheBlocks[i]].dirtyBit = 1;
                            cout << "setting dirty bit " << cacheBlocks[i] << endl;
                        }
                        updateCacheTable(cacheBlocks[i], memoryBlock, mmTable[memoryBlock].tag, cacheTable);
                        cacheTable[cacheBlocks[0]].fifoQueue.push_back(cacheBlocks[i]); //push the cache block changed to the set queue
                        cacheTable[cacheBlocks[0]].lruQueue.push_back(cacheBlocks[i]);  //push the cache block changed to the set queue
                        replacement = false;                                            //memory added to cache, no need for replacement
                        break;                                                          //we have added the value to the cache, so no reason to continue
                    }
                    replacement = true; // gone through all possible blocks and didnt add it to the cache
                }

                if (replacement == true)
                {
                    // LRU implementation
                    if (replacement_policy == "L")
                    {
                        int cacheReplacement = cacheTable[cacheBlocks[0]].lruQueue.front();
                        clearCacheBlock(cacheReplacement, cacheTable);
                        updateCacheTable(cacheReplacement, memoryBlock, mmTable[memoryBlock].tag, cacheTable);
                        if (instruction == 'W')
                        {
                            cacheTable[cacheReplacement].dirtyBit = 1;
                            cout << "setting dirty bit " << cacheBlocks[i] << endl;
                        }
                        cacheTable[cacheBlocks[0]].lruQueue.pop_front();                 //remove the first element from the queue
                        cacheTable[cacheBlocks[0]].lruQueue.push_back(cacheReplacement); //add the new block to the queue
                    }
                    //FIFO implementation
                    if (replacement_policy == "F")
                    {
                        int cacheReplacement = cacheTable[cacheBlocks[0]].fifoQueue.front();
                        clearCacheBlock(cacheReplacement, cacheTable);
                        updateCacheTable(cacheReplacement, memoryBlock, mmTable[memoryBlock].tag, cacheTable);
                        cacheTable[cacheBlocks[0]].fifoQueue.pop_front();                 //remove the first element from the queue
                        cacheTable[cacheBlocks[0]].fifoQueue.push_back(cacheReplacement); //add the new block to the queue
                    }
                }
            }
            if (newReference.hitMiss == "hit")
            {
                for (int i = 0; i < cacheTable[cacheBlocks[0]].lruQueue.size(); i++)
                {
                    if (cacheTable[cacheBlocks[0]].lruQueue.at(i) == cacheBlocks[i])
                    {
                        cacheTable[cacheBlocks[0]].lruQueue.erase(cacheTable[cacheBlocks[0]].lruQueue.begin() + i);
                        cacheTable[cacheBlocks[0]].lruQueue.push_back(cacheBlocks[i]);
                    }
                }
            }
            referenceTable[i] = newReference;
        }
        cout << endl;
        cout << "Simulator Output:" << endl;
        cout << "Total address lines required = " << addressLines << endl;
        cout << "Number of bits for offset = " << offsetBits << endl;
        cout << "Number of bits for index = " << indexBits << endl;
        cout << "Number of bits for tag = " << tagBits << endl;
        cout << "Total cache size required = " << totalCacheSize << " bytes" << endl;
        cout << endl;

        printReferenceTable(setDegree, referenceTable);
        printHitRate(referenceTable);
        printCacheTable(tagBits, cacheTable);
        inFile.close();

        cout << "Continue? (y=yes, n=no): ";
        cin >> repeatProgram;
    }
}
