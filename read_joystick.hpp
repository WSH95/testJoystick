//
// Created by wsh on 2022/2/1.
//

#ifndef TESTJOYSTICK_READ_JOYSTICK_HPP
#define TESTJOYSTICK_READ_JOYSTICK_HPP

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <string>
#include <map>
#include <functional>
#include "my_joystick.hpp"
#include <lcm-cpp.hpp>
#include "joystick_button_lcm.hpp"

/*!
 * from <linux/joystick.h>
 * Types and constants for reading from /dev/js
 */

#define JS_EVENT_BUTTON        0x01    /* button pressed/released */
#define JS_EVENT_AXIS        0x02    /* joystick moved */
#define JSIOCGAXES        _IOR('j', 0x11, __u8)                /* get number of axes */
#define JSIOCGBUTTONS        _IOR('j', 0x12, __u8)                /* get number of buttons */

#define AXIS_VAL_MAX_ABS 32767

struct js_event
{
    uint32_t time;    /* event timestamp in milliseconds */
    int16_t value;    /* value */
    uint8_t type;    /* event type */
    uint8_t number;    /* axis/button number */
};

class ReadJoystick
{
    void _setKeyMap_Bluetooth();

    void _setKeyMap_Wifi();

public:
    ReadJoystick() : _lcm("udpm://239.255.76.67:7667?ttl=255") {}

    ~ReadJoystick() { closeJs(); }

    bool init(const std::string &device_path = "/dev/input/js0", bool blocking = true);

    bool read_event();

    void updateJsData();

    jsDataStruct getJsData() const;

    void lcmPublish();

    void closeJs() { close(_fd); };

private:
    int _fd;
    struct js_event event{};
    jsDataStruct jsData{};
    std::map<uint8_t, uint16_t> buttonMap;
//    std::map<uint8_t, std::reference_wrapper<float>> axisMap;
    lcm::LCM _lcm;
    joystick_button_lcm lcm_js_data{};
    uint8_t _num_buttons = 0;
    uint8_t _num_axes = 0;
//    float tmpXX = 0.0;
//    float tmpYY = 0.0;
};

void ReadJoystick::_setKeyMap_Bluetooth()
{
    // reference to the place in 'jsData.button.components'
    buttonMap[0] = 0x0100;
    buttonMap[1] = 0x0200;
    buttonMap[2] = 0x0400;
    buttonMap[3] = 0x0800;
    buttonMap[4] = 0x0002;
    buttonMap[5] = 0x0001;
    buttonMap[6] = 0x0020;
    buttonMap[7] = 0x0010;
    buttonMap[8] = 0x0008;
    buttonMap[9] = 0x0004;
    buttonMap[15] = 0x0040;
    buttonMap[17] = 0x0080;

//    axisMap.emplace(0, std::reference_wrapper<float>(jsData.LX));
//    axisMap.emplace(1, std::reference_wrapper<float>(jsData.LY));
//    axisMap.emplace(2, std::reference_wrapper<float>(jsData.RX));
//    axisMap.emplace(3, std::reference_wrapper<float>(jsData.RY));
//    axisMap.emplace(4, std::reference_wrapper<float>(jsData.RT));
//    axisMap.emplace(5, std::reference_wrapper<float>(jsData.LT));
//    axisMap.emplace(6, std::reference_wrapper<float>(tmpXX));
//    axisMap.emplace(7, std::reference_wrapper<float>(tmpYY));
}

void ReadJoystick::_setKeyMap_Wifi()
{
    // reference to the place in 'jsData.button.components'
    buttonMap[0] = 0x0100;
    buttonMap[1] = 0x0200;
    buttonMap[3] = 0x0400;
    buttonMap[4] = 0x0800;
    buttonMap[6] = 0x0002;
    buttonMap[7] = 0x0001;
    buttonMap[8] = 0x0020;
    buttonMap[9] = 0x0010;
    buttonMap[10] = 0x0008;
    buttonMap[11] = 0x0004;
    buttonMap[15] = 0x0040;
    buttonMap[2] = 0x0080;

//    axisMap.emplace(0, std::reference_wrapper<float>(jsData.LX));
//    axisMap.emplace(1, std::reference_wrapper<float>(jsData.LY));
//    axisMap.emplace(2, std::reference_wrapper<float>(jsData.RX));
//    axisMap.emplace(3, std::reference_wrapper<float>(jsData.RY));
//    axisMap.emplace(4, std::reference_wrapper<float>(jsData.RT));
//    axisMap.emplace(5, std::reference_wrapper<float>(jsData.LT));
//    axisMap.emplace(6, std::reference_wrapper<float>(tmpXX));
//    axisMap.emplace(7, std::reference_wrapper<float>(tmpYY));
}

bool ReadJoystick::init(const std::string &device_path, bool blocking)
{
    _fd = open(device_path.c_str(), blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK);
    if (_fd < 0)
    {
//        perror("Could not open joystick.\n");
        return false;
    }

    ioctl(_fd, JSIOCGBUTTONS, &_num_buttons);
    ioctl(_fd, JSIOCGAXES, &_num_axes);

    if (_num_axes == 9 && _num_buttons == 18)
        _setKeyMap_Bluetooth();
    else if (_num_axes == 8 && _num_buttons == 20)
        _setKeyMap_Wifi();
    else
    {
//        perror("read error with the number of axes or buttons.\n");
        return false;
    }

    return true;
}

bool ReadJoystick::read_event()
{
    ssize_t len = read(_fd, &event, sizeof(struct js_event));
    if (len != sizeof(event))
    {
        perror("read error");
        return false;
    }

    return true;
}

void ReadJoystick::updateJsData()
{
    switch (event.type)
    {
        case JS_EVENT_BUTTON:
            if (event.value)
                jsData.button.value |= buttonMap[event.number];
            else
                jsData.button.value &= ~(buttonMap[event.number]);
            break;
        case JS_EVENT_AXIS:
            switch (event.number)
            {
                case 0:
                    jsData.LX = -(float) event.value / AXIS_VAL_MAX_ABS;
                    break;
                case 1:
                    jsData.LY = -(float) event.value / AXIS_VAL_MAX_ABS;
                    break;
                case 2:
                    jsData.RX = -(float) event.value / AXIS_VAL_MAX_ABS;
                    break;
                case 3:
                    jsData.RY = -(float) event.value / AXIS_VAL_MAX_ABS;
                    break;
                case 4:
                    jsData.RT = -(float) event.value / (2 * AXIS_VAL_MAX_ABS) - 0.5;
                    break;
                case 5:
                    jsData.LT = (float) event.value / (2 * AXIS_VAL_MAX_ABS) + 0.5;
                    break;
                case 6:
                    if (abs(event.value) < 0.001)
                    {
                        jsData.button.components.left = 0;
                        jsData.button.components.right = 0;
                    }
                    else if (event.value < 0)
                        jsData.button.components.left = 1;
                    else
                        jsData.button.components.right = 1;
                    break;
                case 7:
                    if (abs(event.value) < 0.001)
                    {
                        jsData.button.components.up = 0;
                        jsData.button.components.down = 0;
                    }
                    else if (event.value < 0)
                        jsData.button.components.up = 1;
                    else
                        jsData.button.components.down = 1;
                    break;
                default:
                    break;
            }
            break;
        default:
            break; // Ignore init events.
    }
}

void ReadJoystick::lcmPublish()
{
    lcm_js_data.RB = jsData.button.components.RB;
    lcm_js_data.LB = jsData.button.components.LB;
    lcm_js_data.start = jsData.button.components.start;
    lcm_js_data.select = jsData.button.components.select;
    lcm_js_data.RT = jsData.button.components.RT;
    lcm_js_data.LT = jsData.button.components.LT;
    lcm_js_data.M2 = jsData.button.components.M2;
    lcm_js_data.C = jsData.button.components.C;
    lcm_js_data.A = jsData.button.components.A;
    lcm_js_data.B = jsData.button.components.B;
    lcm_js_data.X = jsData.button.components.X;
    lcm_js_data.Y = jsData.button.components.Y;
    lcm_js_data.up = jsData.button.components.up;
    lcm_js_data.right = jsData.button.components.right;
    lcm_js_data.down = jsData.button.components.down;
    lcm_js_data.left = jsData.button.components.left;

    lcm_js_data.value = (int16_t) jsData.button.value;

    lcm_js_data.LX = jsData.LX;
    lcm_js_data.LY = jsData.LY;
    lcm_js_data.RX = jsData.RX;
    lcm_js_data.RY = jsData.RY;
    lcm_js_data.LT_axis = jsData.LT;
    lcm_js_data.RT_axis = jsData.RT;

    _lcm.publish("joystick data", &lcm_js_data);
}

#endif //TESTJOYSTICK_READ_JOYSTICK_HPP
