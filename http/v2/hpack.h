#ifndef HPACK_H
#define HPACK_H

#include <cstdint>
#include <iostream>
#include <deque>
#include <vector>

namespace Hpack {
    //huffman码
    typedef struct {
        uint32_t nbits;//code的bit数
        uint32_t code;//huffman code
    }huff_sym;

    typedef struct{
        uint8_t state;//当前huffman解码状态，实际是内部哈夫曼树的节点ID
        uint8_t flags;
        uint8_t sym;//符号
    }huff_decode_node ;//huffman解码节点
    typedef enum{
        ACCEPTED=0x1,
        SYMBOL=0x2
    }huff_decode_flag ;//解码标记

    class DynamicTable//动态表
    {

    private:
        std::deque<std::pair<std::string,std::string>> entrys;
        uint32_t header_table_size;
        uint32_t max_header_entry_size;
        uint32_t cur_header_entry_size;

    public:
        DynamicTable():header_table_size(4096),max_header_entry_size(256),cur_header_entry_size(0){

        }

        DynamicTable(const uint32_t& tableSize,const uint32_t& maxSize,std::deque<std::pair<std::string, std::string> > &&entrys )noexcept:
            entrys{std::move(entrys)},header_table_size{tableSize},max_header_entry_size{maxSize},cur_header_entry_size{0}
        {
            for(auto const &pair:this->entrys){
                this->cur_header_entry_size+=pair.first.length()+pair.second.length();
            }
        }

        std::size_t size() const noexcept{
            return entrys.size();
        };
        void addHeader(const std::pair<std::string, std::string> &header)//添加头部字段
        {
            cur_header_entry_size+=header.first.length()+header.second.length();
            entrys.emplace_front(header);//添加至动态表表头

            while(entrys.size()>header_table_size||
                  (max_header_entry_size!=0&&
                   cur_header_entry_size>max_header_entry_size)){
                //当前动态表大小超过maxSize

                auto const& pair=entrys.back();

                cur_header_entry_size-=pair.first.length()+pair.second.length();//减去将要移除的name-value大小
                entrys.pop_back();//移除动态表最后一项

            }

        }
        void addHeader(std::pair<std::string, std::string> &&header){
            this->cur_header_entry_size+=header.first.length()+header.second.length();
            this->entrys.emplace_front(std::move(header));//添加至动态表表头


            while(entrys.size()>header_table_size||
                  (max_header_entry_size!=0&&
                   cur_header_entry_size>max_header_entry_size)){
                //当前动态表大小超过maxSize

                auto const& pair=entrys.back();

                cur_header_entry_size-=pair.first.length()+pair.second.length();//减去将要移除的name-value大小
                entrys.pop_back();//移除动态表最后一项
            }
        }
        void setTableSize(const uint32_t tableSize){
            this->header_table_size=tableSize;

            while(this->entrys.size()>this->header_table_size){
                auto const& pair=entrys.back();

                cur_header_entry_size-=pair.first.length()+pair.second.length();//减去将要移除的name-value大小

                entrys.pop_back();//移除动态表最后一项
            }
        }
        void setMaxSize(const uint32_t maxSize){
            this->max_header_entry_size=maxSize;

            while(this->max_header_entry_size!=0&&this->cur_header_entry_size>this->max_header_entry_size){
                auto const& pair=entrys.back();

                cur_header_entry_size-=pair.first.length()+pair.second.length();//减去将要移除的name-value大小

                entrys.pop_back();//移除动态表最后一项
            }
        }

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
