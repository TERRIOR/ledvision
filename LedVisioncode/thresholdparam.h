#ifndef THRESHOLDPARAM_H
#define THRESHOLDPARAM_H


class thresholdparam
{
public:
    thresholdparam();
    thresholdparam(float,float);
    void setdownup(float,float);
    bool inrange();
    float now() const;
    void setNow(float now);
    float down() const;
    void setDown(float down);

    float up() const;
    void setUp(float up);

private:
    float m_now=0;
    float m_down=-1000;
    float m_up=1000;

};

#endif // THRESHOLDPARAM_H
