#ifndef SETTINGSFRAME_H
#define SETTINGSFRAME_H

#include<string>
#include"frame.h"
struct settings_entry{
    int32_t settings_id;//setting id
    uint32_t value;//
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
