#ifndef _XSI_SEGMENT_H_
#define _XSI_SEGMENT_H_

#include "segment.h"

class XsiSegment : public Segment {
public:
    explicit XsiSegment(uint64_t channel_id);
    virtual ~XsiSegment();

    static const char* Type() { return "xsi"; }

private:
    void Reset() override;
    bool Remove() override;
    bool OpenOnly() override;
    bool OpenOrCreate() override;

    key_t key_;
}


#endif // _XSI_SEGMENT_H_