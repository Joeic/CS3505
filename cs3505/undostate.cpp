#include "undostate.h"

///
/// \brief UndoState::UndoState constructor.
/// \param _type what change does this UndoState represent?
///
UndoState::UndoState(UndoStateType _type)
    : type(_type)
{

}

///
/// \brief UndoState::forFrameChange static helper to construct an UndoState representing a change to a frame's canvas
/// \param index which frame?
/// \param old_frame old frame data (before the change)
/// \param new_frame new frame data (after the change)
/// \return UndoState
///
UndoState UndoState::forFrameChange(int index, std::shared_ptr<Frame> old_frame, std::shared_ptr<Frame> new_frame)
{
    UndoState s(UndoStateType::FRAME_CHANGE);
    s.frame_start_index = index;
    s.old_frame = old_frame;
    s.new_frame = new_frame;
    return s;
}

///
/// \brief UndoState::forFrameAdd static helper to construct an UndoState representing a newly-inserted frame
/// \param index where is the frame, after insertion?
/// \param frame frame data
/// \return UndoState
///
UndoState UndoState::forFrameAdd(int index, std::shared_ptr<Frame> frame)
{
    UndoState s(UndoStateType::FRAME_ADD);
    s.frame_start_index = index;
    s.new_frame = frame;
    return s;
}

///
/// \brief UndoState::forFrameDelete static helper to construct an UndoState representing the removal of a frame
/// \param index where was the frame?
/// \param deleted_frame old frame data
/// \return UndoState
///
UndoState UndoState::forFrameDelete(int index, std::shared_ptr<Frame> deleted_frame)
{
    UndoState s(UndoStateType::FRAME_DELETE);
    s.frame_start_index = index;
    s.old_frame = deleted_frame;
    return s;
}

///
/// \brief UndoState::forFrameReinsert static helper to construct an UndoState representing moving a frame
/// \param from_index where was the frame before the change?
/// \param to_index where is it after the change?
/// \param frame frame data
/// \return UndoState
///
UndoState UndoState::forFrameReinsert(int from_index, int to_index, std::shared_ptr<Frame> frame)
{
    UndoState s(UndoStateType::FRAME_REINSERT);
    s.frame_start_index = from_index;
    s.frame_end_index = to_index;
    s.new_frame = frame;
    return s;
}
