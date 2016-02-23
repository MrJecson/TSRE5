#include "FileBuffer.h"

FileBuffer::FileBuffer() {
    this->off = 0;
}

FileBuffer::FileBuffer(unsigned char * data, int nLength) {
    this->data = data;
    this->length = nLength;
    this->off = 0;
}

FileBuffer::FileBuffer(const FileBuffer& orig) {
}

FileBuffer::~FileBuffer() {
    delete[] this->data;
}

int FileBuffer::getInt() {
    this->off += 4;
    return *((int*) & this->data[this->off - 4]);
}

int FileBuffer::getToken(){
    this->off += 4;
    return (*((int*) & this->data[this->off - 4]) - this->tokenOffset);
}
void FileBuffer::setTokenOffset(int val){
    this->tokenOffset = val;
}

unsigned int FileBuffer::getUint() {
    this->off += 4;
    return *((unsigned int*) & this->data[this->off - 4]);
}

unsigned short int FileBuffer::getShort() {
    this->off += 2;
    //return this->data[this->off - 2]*256 + 0;
    return *((unsigned short int*) & this->data[this->off - 2]);
}

short int FileBuffer::getSignedShort() {
    this->off += 2;
    //return this->data[this->off - 2]*256 + 0;
    return *((short int*) & this->data[this->off - 2]);
}

float FileBuffer::getFloat() {
    this->off += 4;
    return *(float*) & this->data[this->off - 4];
}

unsigned char FileBuffer::get() {
    return this->data[this->off++];
}

QString* FileBuffer::getString(int start, int end) {
    QString* s = new QString();

    for (int i = start; i < end; i += 2) {
        if (data[i] == 13) continue;
        *s += QChar(data[i], data[i + 1]);
    }
    return s;

}

void FileBuffer::findToken(int id) {
    int s;
    while (length > off) {
        s = (int) getInt();
        if (s == id)
            return;
        s = getInt();
        off += s;
    }
    return;
}