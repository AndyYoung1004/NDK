//
// Created by Administrator on 2023/9/12 0012.
//

#ifndef TESTNDK_MEDIAPLAYER_H
#define TESTNDK_MEDIAPLAYER_H

class MediaPlayer {
public:
    MediaPlayer(){};
    virtual ~MediaPlayer(){};

    virtual void Init() = 0;
    virtual void UnInit() = 0;
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SeekTo() = 0;
};


#endif //TESTNDK_MEDIAPLAYER_H
