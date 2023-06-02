#ifndef SETTINGSFRAME_H
#define SETTINGSFRAME_H

#include<string>
#include"frame.h"
struct settings_entry{
    int32_t settings_id;//setting id
    uint32_t value;//
};
enum settings_id{//settings id参数定义
    SETTINGS_HEADER_TABLE_SIZE = 0x01,
    SETTINGS_ENABLE_PUSH = 0x02,
    SETTINGS_MAX_CONCURRENT_STREAMS = 0x03,
    SETTINGS_INITIAL_WINDOW_SIZE = 0x04,
    SETTINGS_MAX_FRAME_SIZE = 0x05,
    SETTINGS_MAX_HEADER_LIST_SIZE = 0x06,
    SETTINGS_ENABLE_CONNECT_PROTOCOL = 0x08,
    SETTINGS_NO_RFC7540_PRIORITIES = 0x09
};

class SettingsFrame : public Frame
{
public:
    SettingsFrame(settings_entry *iv,std::size_t niv);
private:
    std::size_t m_niv;//总共键值对的个数
    settings_entry *m_iv;//键值对
};

#endif // SETTINGSFRAME_H
