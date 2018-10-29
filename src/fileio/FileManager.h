#ifndef FILE_MANAGER
#define FILE_MANAGER


#include "../utils/pagedef.h"

//#include "../MyLinkList.h"
using namespace std;

class MyBitMap;

class FileManager {
private:
    //FileTable* ftable;
    int fd[MAX_FILE_NUM];
    MyBitMap *fm;
    MyBitMap *tm;

    int _createFile(const char *name);

    int _openFile(const char *name, int fileID);
    /*
     * FilManager构造函数
     */
    FileManager();

public:

    static FileManager &getInstance();

    /*
     * @函数名writePage
     * @参数fileID:文件id，用于区别已经打开的文件
     * @参数pageID:文件的页号
     * @参数buf:存储信息的缓存(4字节无符号整数数组)
     * @参数off:偏移量
     * 功能:将buf+off开始的2048个四字节整数(8kb信息)写入fileID和pageID指定的文件页中
     * 返回:成功操作返回0
     */
    int writePage(int fileID, int pageID, BufType buf, int off);

    /*
     * @函数名readPage
     * @参数fileID:文件id，用于区别已经打开的文件
     * @参数pageID:文件页号
     * @参数buf:存储信息的缓存(4字节无符号整数数组)
     * @参数off:偏移量
     * 功能:将fileID和pageID指定的文件页中2048个四字节整数(8kb)读入到buf+off开始的内存中
     * 返回:成功操作返回0
     */
    int readPage(int fileID, int pageID, BufType buf, int off);

    /*
     * @函数名closeFile
     * @参数fileID:用于区别已经打开的文件
     * 功能:关闭文件
     * 返回:操作成功，返回0
     */
    int closeFile(int fileID);

    /*
     * @函数名createFile
     * @参数name:文件名
     * 功能:新建name指定的文件名
     * 返回:操作成功，返回true
     */
    int createFile(const char *name);

    /*
     * @函数名openFile
     * @参数name:文件名
     * @参数fileID:函数返回时，如果成功打开文件，那么为该文件分配一个id，记录在fileID中
     * 功能:打开文件
     * 返回:如果成功打开，在fileID中存储为该文件分配的id，返回true，否则返回false
     */
    int openFile(const char *name, int &fileID);

    int deleteFile(const char *name);

    int newType();


    void closeType(int typeID);

    void shutdown();

    ~FileManager();
};

#endif
