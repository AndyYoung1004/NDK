//
// Created by Administrator on 2023/9/12 0012.
//

#ifndef TESTNDK_HWCODECPLAYER_H
#define TESTNDK_HWCODECPLAYER_H

#include "MediaPlayer.h"

class HWCodecPlayer : public MediaPlayer {
public:
    HWCodecPlayer(){};
    virtual ~HWCodecPlayer(){};

    virtual void Init();
    virtual void UnInit();
    virtual void Play();
    virtual void Pause();
    virtual void Stop();
    virtual void SeekTo();
};

#endif //TESTNDK_HWCODECPLAYER_H
