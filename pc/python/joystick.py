import pygame
import sys


class Joystick:

    linear_axis_idx = 0
    angular_axis_idx = 1
    led_on_button_idx = 2
    led_off_button_idx = 3
    init_button_idx = 4

    def __init__(self,
                 linear_axis : int,
                 invert_linear_axis : bool,
                 angular_axis : int,
                 invert_angular_axis : bool,
                 allow_button : int,
                 led_on_button : int,
                 led_off_button : int,
                 init_button : int) -> None:
        
        pygame.init()
        pygame.joystick.init()

        if pygame.joystick.get_count() == 0:
            print("No joysticks found")
            sys.exit()

        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()

        self.axes = self.joystick.get_numaxes()
        self.buttons = self.joystick.get_numbuttons()
        self.hats = self.joystick.get_numhats()

        if linear_axis >= self.axes or angular_axis >= self.axes:
            print('wrong axes setup')
            sys.exit()
        if (allow_button >= self.buttons or led_on_button >= self.buttons or led_off_button >= self.buttons or init_button >= self.buttons):
            print('wrong buttons setup')
            sys.exit()

        self.linear_axis = linear_axis
        self.angular_axis = angular_axis
        self.allow_button = allow_button
        self.led_on_button = led_on_button
        self.led_off_button = led_off_button
        self.init_button = init_button

        self.invert_linear = invert_linear_axis
        self.invert_angular = invert_angular_axis

        self.allow_move = False
        
        self.verbose = True

    def __delete__(self):
        pygame.quit()

    def spin(self):
        state = [0, 0, False, False, False]

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return None
            
            if event.type == pygame.JOYAXISMOTION:
                v = self.joystick.get_axis(self.linear_axis)
                w = self.joystick.get_axis(self.angular_axis)
                state[self.linear_axis_idx] = int(round(v if not self.invert_linear else -v, 2) * 100)
                state[self.angular_axis_idx] = int(round(w if not self.invert_angular else -w, 2) * 100)

            if event.type == pygame.JOYBUTTONDOWN:
                if self.joystick.get_button(self.allow_button):
                    self.allow_move = True
                state[self.led_on_button_idx] = True if self.joystick.get_button(self.led_on_button) else False
                state[self.led_off_button_idx] = True if self.joystick.get_button(self.led_off_button) else False
                state[self.init_button_idx] = True if self.joystick.get_button(self.init_button) else False

            if event.type == pygame.JOYBUTTONUP:
                if not self.joystick.get_button(self.allow_button):
                    self.allow_move = False

            if event.type == pygame.JOYHATMOTION:
                pass

            if not self.allow_move:
                state[self.linear_axis_idx] = 0
                state[self.angular_axis_idx] = 0

            print(state)
            return state
        
    def enableVerbose(self):
        self.verbose = True

    def disableVerbose(self):
        self.verbose = False


if __name__ == '__main__':
    linear = 1
    angular = 0
    allow = 0
    led_on = 3
    led_off = 1
    init = 4
    joystick = Joystick(linear, True, angular, False, allow, led_on, led_off, init)

    while True:
        joystick.spin()
