#include "thresholdparam.h"

thresholdparam::thresholdparam()
{

}

thresholdparam::thresholdparam(float down, float up):m_down(down),m_up(up)
{

}

void thresholdparam::setdownup(float down, float up)
{
    m_down=down;
    m_up=up;
}

bool thresholdparam::inrange()
{
    if(m_now>m_down&&m_now<m_up){
        return true;
    }else{
        //cout<<"fail"<<endl;
        showallparam();
        return false;
    }

}

float thresholdparam::now() const
{
    return m_now;
}

void thresholdparam::setNow(float now)
{
    m_now = now;
}

float thresholdparam::down() const
{
    return m_down;
}

void thresholdparam::setDown(float down)
{
    m_down = down;
}


float thresholdparam::up() const
{
    return m_up;
}

void thresholdparam::setUp(float up)
{
    m_up = up;
}

void thresholdparam::showallparam()
{
    cout<<m_down<<" "<<m_now<<" "<<m_up<<endl;
}
