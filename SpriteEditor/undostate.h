#ifndef UNDOSTATE_H
#define UNDOSTATE_H

#include "frame.h"
#include <functional>
#include <memory>

///
/// \brief The UndoStateType enum represents the type of change held in an UndoState.
///
enum class UndoStateType
{
    // The frame at frame_start_index has new data (previously old_frame, now new_frame)
    FRAME_CHANGE,

    // A new frame has been inserted at frame_start_index, with new data in new_frame
    FRAME_ADD,

    // A frame has been deleted from frame_start_index, with deleted data in old_frame
    FRAME_DELETE,

    // A frame has been removed from frame_start_index and subsequently inserted at frame_end_index, with data unchanged in new_frame
    FRAME_REINSERT
};

///
/// \brief The UndoState class represents a change made to the Animation and its Frames
/// \author Kyle Holland
///
class UndoState
{
public:
    UndoState(UndoStateType _type);

    static UndoState forFrameChange(int index, std::shared_ptr<Frame> old_frame, std::shared_ptr<Frame> new_frame);
    static UndoState forFrameAdd(int index, std::shared_ptr<Frame> frame);
    static UndoState forFrameDelete(int index, std::shared_ptr<Frame> deleted_frame);
    static UndoState forFrameReinsert(int from_index, int to_index, std::shared_ptr<Frame> frame);

    UndoStateType type;
    int frame_start_index = -1;
    int frame_end_index = -1;
    std::shared_ptr<Frame> old_frame;
    std::shared_ptr<Frame> new_frame;
};

#endif // UNDOSTATE_H
