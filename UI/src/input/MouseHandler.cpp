#include "MouseHandler.hpp"

void MouseHandler::attachTo(const std::string& windowName)
{
    cv::setMouseCallback(
        windowName,
        MouseHandler::mouseCallback,
        this
    );
}


void MouseHandler::setOnClick(ClickCallback callback)
{
    callback_ = callback;
}

void MouseHandler::setOnRightClick(ClickCallback callback)
{
    rightClickCallback_ = callback;
}

void MouseHandler::mouseCallback(int event, int x, int y, int flags, void* userdata)
{
    if (event != cv::EVENT_LBUTTONDOWN && event != cv::EVENT_RBUTTONDOWN)
        return;

    auto* handler = static_cast<MouseHandler*>(userdata);

    auto position = handler->translator_.pixelToCell(x, y);

    if (!position.has_value())
    {
        if (event == cv::EVENT_LBUTTONDOWN && handler->outsideClickCallback_)
            handler->outsideClickCallback_();
        return;
    }

    if (event == cv::EVENT_LBUTTONDOWN)
    {
        if (handler->callback_)
            handler->callback_(position.value());
    }
    else if (event == cv::EVENT_RBUTTONDOWN)
    {
        if (handler->rightClickCallback_)
            handler->rightClickCallback_(position.value());
    }
}

void MouseHandler::setOnOutsideClick(VoidCallback callback)
{
    outsideClickCallback_ = std::move(callback);
}