#include "encoder.h"

Encoder::Encoder()
{

}

void Encoder::packInteger(std::vector<char> &dest, uint64_t num, const uint8_t prefix)
{//整数编码
    const uint64_t k = (1 << prefix) - 1;

    if(num<k){
        dest.emplace_back(num);
        return;
    }

    dest.emplace_back(k);
    num-=k;
    for(;;){
        if(num<128){
            dest.emplace_back(num);
            break;
        }

        dest.emplace_back(0x80|(num&0x7f));

        num>>=7;
        if(num==0)
            break;
    }

}

void Encoder::packIndex(std::vector<char> &dest, const std::size_t index)
{
    const std::size_t head=dest.size();
    packInteger(dest,index,7);
    dest[head]|=0x80;
}

std::tuple<std::size_t, bool> Encoder::findHeaderInTable(const std::pair<std::string, std::string> &header, const Hpack::DynamicTable &dynamicTable)
{//返回索引
    std::tuple<std::size_t, bool> res {0,false};

    std::size_t& index=std::get<std::size_t>(res);//获取索引
    bool& is_full_match=std::get<bool>(res);//是否匹配

    for(std::size_t i=0;i<Hpack::getStaticTableSize();++i){
        auto const&pair=Hpack::static_table[i];

        if(pair.first==header.first){
            index=i+1;
            if(pair.second==header.second){
                is_full_match=true;
                return res;
            }
        }else if (0 != index) {
            break;
        }
    }
    for(std::size_t i=0;i<dynamicTable.size();++i){
        auto const& pair=dynamicTable[i];

        if(pair.first==header.first){
            index=i+1+Hpack::getStaticTableSize();
            if(pair.second==header.second){
                is_full_match=true;
                break;
            }
        }
    }

    return res;
}
