#ifndef HPACK_H
#define HPACK_H

#include <cstdint>
#include <iostream>
#include <deque>
#include <vector>

namespace Hpack {
    typedef struct {
        uint32_t nbits;//code的bit数
        uint32_t code;//huffman code
    }huff_sym;

    typedef struct{
        uint8_t state;//当前huffman解码状态，实际是内部哈夫曼树的节点ID
        uint8_t flags;
        uint8_t sym;//符号
    }huff_code ;
    typedef enum{
        ACCEPTED=0x1,
        SYMBOL=0x2
    }huff_decode_flag ;

    class DynamicTable//动态表
    {

    private:
        std::deque<std::pair<std::string,std::string>> entrys;
        uint32_t header_table_size;
        uint32_t max_header_entry_size;
        uint32_t cur_header_entry_size;

    public:
        DynamicTable(const uint32_t& tableSize,const uint32_t& maxSize,std::deque<std::pair<std::string, std::string> > &&entrys )noexcept;

        std::size_t size() const noexcept{
            return entrys.size();
        };
        void addHeader(const std::pair<std::string, std::string> &header);//添加头部字段
        void addHeader(std::pair<std::string, std::string> &&header);
        void setTableSize(const uint32_t tableSize);
        void setMaxSize(const uint32_t maxSize);

        const std::pair<std::string, std::string>& operator[](const std::size_t index)const noexcept{
            return this->entrys[index];
        };//[]访问动态表内数据

        const std::deque<std::pair<std::string, std::string> >& getEntrys() const noexcept{
            return this->entrys;
        };

    };

    static const std::pair<std::string, std::string> static_table[] {//静态表
            {":authority", ""},
            {":method", "GET"},
            {":method", "POST"},
            {":path", "/"},
            {":path", "/index.html"},
            {":scheme", "http"},
            {":scheme", "https"},
            {":status", "200"},
            {":status", "204"},
            {":status", "206"},
            {":status", "304"},
            {":status", "400"},
            {":status", "404"},
            {":status", "500"},
            {"accept-charset", ""},
            {"accept-encoding", "gzip, deflate"},
            {"accept-language", ""},
            {"accept-ranges", ""},
            {"accept", ""},
            {"access-control-allow-origin", ""},
            {"age", ""},
            {"allow", ""},
            {"authorization", ""},
            {"cache-control", ""},
            {"content-disposition", ""},
            {"content-encoding", ""},
            {"content-language", ""},
            {"content-length", ""},
            {"content-location", ""},
            {"content-range", ""},
            {"content-type", ""},
            {"cookie", ""},
            {"date", ""},
            {"etag", ""},
            {"expect", ""},
            {"expires", ""},
            {"from", ""},
            {"host", ""},
            {"if-match", ""},
            {"if-modified-since", ""},
            {"if-none-match", ""},
            {"if-range", ""},
            {"if-unmodified-since", ""},
            {"last-modified", ""},
            {"link", ""},
            {"location", ""},
            {"max-forwards", ""},
            {"proxy-authenticate", ""},
            {"proxy-authorization", ""},
            {"range", ""},
            {"referer", ""},
            {"refresh", ""},
            {"retry-after", ""},
            {"server", ""},
            {"set-cookie", ""},
            {"strict-transport-security", ""},
            {"transfer-encoding", ""},
            {"user-agent", ""},
            {"vary", ""},
            {"via", ""},
            {"www-authenticate", ""},
        };

}

#endif // HPACK_H
