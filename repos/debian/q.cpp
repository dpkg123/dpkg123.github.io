#include <cstdio>
#include <string>
#include <cstring>
#include <cstdint>

using std::string;

//Header :
//Count 
//Unknown

typedef struct Header
{
    uint32_t Count;
    uint32_t InfoSize;
} Header;

//FileInfo
typedef struct FileInfo
{
    uint32_t size;
    uint32_t offset;
    string name;
} FileInfo;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        return -1;
    }

    FILE* file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        return -1;
    }

    Header* header = new Header;
    fread(header, 1, sizeof(Header), file);

    fseek(file, 8, SEEK_SET);
    FileInfo* FilePool = new FileInfo[header->Count];
    char* InfoBuffer = new char[header->InfoSize];
    fread(InfoBuffer, 1, header->InfoSize, file);

    uint32_t iPos = 0;
    for (unsigned int i = 0; i < header->Count; i++)
    {
        uint32_t Offset;
        uint32_t Size;
        Size = *(uint32_t*)(InfoBuffer + iPos);
        iPos += 4;
        Offset = *(uint32_t*)(InfoBuffer + iPos);
        iPos += 4;
        FilePool[i].size = Size;
        FilePool[i].offset = Offset;
        FilePool[i].name = string((char*)(InfoBuffer + iPos));
        iPos += (strlen((char*)(InfoBuffer + iPos)) + 1);
    }

    for (unsigned int i = 0; i < header->Count; i++)
    {
        fseek(file, FilePool[i].offset + sizeof(Header) + header->InfoSize, SEEK_SET);
        char* pBuffer = new char[FilePool[i].size];
        fread(pBuffer, 1, FilePool[i].size, file);

        FILE* out = fopen(FilePool[i].name.c_str(), "wb");
        fwrite(pBuffer, 1, FilePool[i].size, out);
        fclose(out);
        delete[] pBuffer;
    }

    delete[] InfoBuffer;
    delete[] FilePool;
    delete header;
    fclose(file);

    return 0;
}
