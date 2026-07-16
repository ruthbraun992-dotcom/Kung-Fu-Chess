#pragma once

#include <opencv2/opencv.hpp>
#include <functional>
#include "ClickTranslator.hpp"
#include "Position.hpp"
#include "Board.hpp"


class MouseHandler {
public:

    using ClickCallback =
        std::function<void(const Position&)>;


    explicit MouseHandler(ClickTranslator& translator)
        : translator_(translator) {}


    void attachTo(const std::string& windowName);


    void setOnClick(ClickCallback callback);


private:

    static void mouseCallback(
        int event,
        int x,
        int y,
        int flags,
        void* userdata
    );


    ClickTranslator& translator_;

    ClickCallback callback_;

};