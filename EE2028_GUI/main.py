try:
    import pygame
    import math
    import random
    from pygame.locals import *
    import serial
except ImportError as ImpErr:
    print("Check your imports !")
    print(str(ImpErr))


class Initialise(object):
    # Colour definitions (Red ,Green, Blue)
    # All the below code are class variables
    BLACK = (0, 0, 0)
    WHITE = (255, 255, 255)
    RED = (255, 0, 0)
    GREEN = (0, 255, 0)
    BLUE = (0, 0, 255)
    YELLOW = (255, 255, 0)
    CYAN = (0, 255, 255)
    BROWN = (83, 91, 36)
    SILVER = (192, 192, 192)

    pygame.init()
    pygame.display.set_caption("Dashboard")
    clock = pygame.time.Clock()
    resolution = (1024, 600)
    screen = pygame.display.set_mode(resolution, pygame.RESIZABLE)
    ser = serial.Serial('COM6', 115200, 8, 'N', 1, timeout=5)


class Text(Initialise):
    def message_display(self, text, x_position, y_position):
        largeText = pygame.font.Font("freesansbold.ttf", 20)
        # The text is inside a rectangle and can be referenced by a rectangle.
        textSurface = largeText.render(text, True, Initialise.CYAN)

        # TextSurface, TextRect = text_objects(text, largeText)
        TextRect = textSurface.get_rect()
        TextRect.center = (x_position, y_position)
        Initialise.screen.blit(textSurface, TextRect)

    def title(self, text, x_position, y_position):
        largeText = pygame.font.Font("freesansbold.ttf", 40)
        # The text is inside a rectangle and can be referenced by a rectangle.
        textSurface = largeText.render(text, True, Initialise.GREEN)

        # TextSurface, TextRect = text_objects(text, largeText)
        TextRect = textSurface.get_rect()
        TextRect.center = (x_position, y_position)
        Initialise.screen.blit(textSurface, TextRect)


class Gyro_X(Text):
    def draw_rect(self, gyro_x_value):
        # pygame.draw.rect(Initialise.screen, Battery.SILVER, (100, 100, 15, 30))
        pygame.draw.rect(Initialise.screen, Gyro_X.WHITE,
                         (100, 200, 200, 55), 3)

        # pygame.draw.rect(screen, color, (x,y,width,height), thickness)
        # pygame.draw.rect(self.screen, Battery.GREEN, (802, 27, 145, 97))
        if (abs(gyro_x_value) >= 190):
            gyro_x_value = 190
        pygame.draw.rect(Initialise.screen, Initialise.GREEN,
                         (105, 205, (abs(gyro_x_value)), 45), 117)
        # elif ((battery_value > 42) and (battery_value <= 70)):
        #     pygame.draw.rect(Initialise.screen, Battery.YELLOW,
        #                      (802, 27, battery_value, 117))
        # elif (battery_value >= 0 and battery_value <= 42):
        #     pygame.draw.rect(Initialise.screen, Initialise.RED,
        #                      (802, 27, battery_value, 117))
        #     self.bolt_image = pygame.image.load("bolted.png")
        #     self.bolt_image = pygame.transform.scale(self.bolt_image,
        #                                              (100, 110))
        #     Initialise.screen.blit(self.bolt_image, (830, 33))
        #     Text.message_display(self, text="LOW Battery !!!", x_position=865,
        #                          y_position=200)


class Gyro_Y(Text):
    def draw_rect(self, gyro_y_value):
        # pygame.draw.rect(Initialise.screen, Battery.SILVER, (100, 100, 15, 30))

        pygame.draw.rect(Initialise.screen, Gyro_X.WHITE,
                         (100, 100, 200, 55), 3)

        # pygame.draw.rect(screen, color, (x,y,width,height), thickness)
        # pygame.draw.rect(self.screen, Battery.GREEN, (802, 27, 145, 97))
        if (abs(gyro_y_value) >= 190):
            gyro_y_value = 190
        pygame.draw.rect(Initialise.screen, Initialise.GREEN,
                         (105, 105, abs(gyro_y_value), 45), 117)


class Mag_Z(Text):
    def draw_rect(self, mag_z_value):
        # pygame.draw.rect(Initialise.screen, Battery.SILVER, (100, 100, 15, 30))

        pygame.draw.rect(Initialise.screen, Mag_Z.WHITE,
                         (360, 100, 200, 55), 3)

        # pygame.draw.rect(screen, color, (x,y,width,height), thickness)
        # pygame.draw.rect(self.screen, Battery.GREEN, (802, 27, 145, 97))
        if (abs(mag_z_value) >= 190):
            mag_z_value = 190
        pygame.draw.rect(Initialise.screen, Initialise.GREEN,
                         (365, 105, abs(mag_z_value), 45), 117)


class Mag_Y(Text):
    def draw_rect(self, mag_y_value):
        # pygame.draw.rect(Initialise.screen, Battery.SILVER, (100, 100, 15, 30))

        pygame.draw.rect(Initialise.screen, Gyro_Y.WHITE,
                         (360, 200, 200, 55), 3)

        # pygame.draw.rect(screen, color, (x,y,width,height), thickness)
        # pygame.draw.rect(self.screen, Battery.GREEN, (802, 27, 145, 97))
        if (abs(mag_y_value) >= 190):
            mag_y_value = 190
        pygame.draw.rect(Initialise.screen, Initialise.GREEN,
                         (365, 205, abs(mag_y_value), 45), 117)


class Mag_X(Text):
    def draw_rect(self, mag_x_value):
        # pygame.draw.rect(Initialise.screen, Battery.SILVER, (100, 100, 15, 30))

        pygame.draw.rect(Initialise.screen, Mag_X.WHITE,
                         (360, 300, 200, 55), 3)

        # pygame.draw.rect(screen, color, (x,y,width,height), thickness)
        # pygame.draw.rect(self.screen, Battery.GREEN, (802, 27, 145, 97))
        if (abs(mag_x_value) >= 190):
            mag_x_value = 190
        pygame.draw.rect(Initialise.screen, Initialise.GREEN,
                         (365, 305, abs(mag_x_value), 45), 117)


class Accel(Text):
    def draw_rect(self, accel_value):
        # pygame.draw.rect(Initialise.screen, Battery.SILVER, (100, 100, 15, 30))

        pygame.draw.rect(Initialise.screen, Mag_X.WHITE,
                         (975, 100, 210, 55), 3)

        # pygame.draw.rect(screen, color, (x,y,width,height), thickness)
        # pygame.draw.rect(self.screen, Battery.GREEN, (802, 27, 145, 97))
        if (abs(accel_value) >= 200):
            accel_value = 200
        pygame.draw.rect(Initialise.screen, Initialise.GREEN,
                         (980, 105, abs(accel_value), 45), 117)


class Warning(Initialise):
    def load_image(self):
        self.bolted = pygame.image.load("bolted.png")
        self.bolted_image = pygame.transform.scale(self.bolted,
                                                   (100, 250))
        Initialise.screen.blit(self.bolted_image, (155, 400))


class Speedometer(Initialise):

    def load_image(self):
        self.speedometer_image = pygame.image.load("speed.png")
        self.speedometer_image = pygame.transform.scale(self.speedometer_image,
                                                        (700, 450))
        Initialise.screen.blit(self.speedometer_image, (155, -40))

    def draw_arc(self, speed_value):
        # pygame.draw.arc()
        # Good example arc below ->
        # pygame.draw.arc(self.screen, Speedometer.YELLOW,
        # (235, 75, 525, 525), math.radians(-42), math.radians(223), 4)

        # draw a partial section of an ellipse
        # arc(Surface, color, Rect, start_angle, stop_angle, width=1) -> Rect
        """ Draws an elliptical arc on the Surface.
        The rect argument is the area that the ellipse will fill.
        The two angle arguments are the initial and final
        angle in radians, with the zero on the right. The width argument is the
        thickness to draw the outer edge.

        TAKE NOTE: <Worth mentioning>
        the initial angle must be less than the final angle;
        otherwise it will draw the full ellipse. """
        pygame.draw.arc(Initialise.screen, Speedometer.YELLOW,
                        (277, 5, 450, 400), math.radians(speed_value),
                        math.radians(224), 5)


class Pressure(Text):
    def draw_arc(self, pressure_value):
        if (pressure_value >= 0 and pressure_value < 180):
            pygame.draw.arc(Initialise.screen, Initialise.WHITE,
                            (700, 100, 250, 250),
                            math.radians(pressure_value), math.radians(180),
                            25)


class Humidity(Text):
    def draw_arc(self, hum_value):
        pygame.draw.arc(Initialise.screen, Initialise.BROWN,
                        (700, 280, 250, 250),
                        math.radians(hum_value), math.radians(180),
                        25)


class Temperature(Text):
    def draw_arc(self, temp_value):
        pygame.draw.arc(Initialise.screen, Initialise.BLUE,
                        (700, 450, 250, 250),
                        math.radians(temp_value), math.radians(180),
                        25)


class Fluxer(Text):
    def draw_rect(self, fluxer_value):
        # pygame.draw.rect(Initialise.screen, Battery.SILVER, (100, 100, 15, 30))

        pygame.draw.rect(Initialise.screen, Mag_X.WHITE,
                         (975, 200, 210, 55), 3)

        # pygame.draw.rect(screen, color, (x,y,width,height), thickness)
        # pygame.draw.rect(self.screen, Battery.GREEN, (802, 27, 145, 97))
        if (abs(fluxer_value) >= 200):
            fluxer_value = 200
        pygame.draw.rect(Initialise.screen, Initialise.YELLOW,
                         (980, 205, fluxer_value, 45), 117)


class UpdateValues(Initialise):
    def __init__(self):
        self.gyro_x_raw_value = 0
        self.gyro_y_raw_value = 0
        self.gyro_x_value = 0  # Used to display values
        self.gyro_y_value = 0  # Used to display values

        self.mag_x_raw_value = 0
        self.mag_y_raw_value = 0
        self.mag_z_raw_value = 0
        self.mag_x_value = 0  # Used to display values
        self.mag_y_value = 0  # Used to display values
        self.mag_z_value = 0  # Used to display values

        self.pressure_raw_value = 0
        self.pressure_value = 0  # Used to display values

        self.hum_raw_value = 0
        self.hum_value = 0  # Used to display values

        self.temp_raw_value = 30
        self.temp_value = -2*(self.temp_raw_value) + \
            140  # Used to display values

        self.accel_raw_value = 5
        self.accel_value = self.accel_raw_value * 20

        self.fluxer_raw_value = 10
        self.fluxer_value = 200

        self.mode = 1
        # 1 - Exploration Mode
        # 2 - Exploration Warning Mode
        # 3 - Battle Mode
        # 4 - Battle Warning Mode
        # 5 - Fluxer

    def transferValues(self):
        if (Initialise.ser.in_waiting):
            try:
                values = Initialise.ser.readline()
                # print('values', values)
                decoded = values.decode("utf-8")
                # print('Decoded', decoded)
                decoded = str(decoded)
                split_decoded = decoded.split(",")
                print(split_decoded)

                self.mode = int(split_decoded[0].strip())
                if (self.mode == 1):
                    self.gyro_x_raw_value = float(
                        split_decoded[1].strip()[2:6])
                    self.gyro_y_raw_value = float(
                        split_decoded[1].strip()[7:12])

                    self.mag_x_raw_value = float(split_decoded[2].strip()[2:8])
                    self.mag_y_raw_value = float(
                        split_decoded[2].strip()[9:15])
                    self.mag_z_raw_value = float(
                        split_decoded[2].strip()[16:22])

                    self.pressure_raw_value = float(
                        split_decoded[3].strip()[2:11])

                    self.hum_raw_value = float(split_decoded[4].strip()[2:8])

                    # Calculations

                    self.pressure_value = (-0.012 *
                                           self.pressure_raw_value) + 1260
                    self.hum_value = (-2.25 *
                                      self.hum_raw_value) + 247.5
                    self.temp_value = (-2 *
                                       self.temp_raw_value) + 140
                    self.gyro_x_value = (self.gyro_x_raw_value * 10)
                    self.gyro_y_value = (self.gyro_y_raw_value * 10)
                    self.mag_x_value = (self.mag_x_raw_value * 50)
                    self.mag_y_value = (self.mag_y_raw_value * 50)
                    self.mag_z_value = (
                        self.mag_z_raw_value * 50)  # 200 / 4 = 50
                    self.accel_value = self.accel_raw_value * 20
                    self.fluxer_value = self.fluxer_raw_value * 20

                elif (self.mode == 2):
                    pass
                elif (self.mode == 3):  # Battle Mode

                    self.temp_raw_value = float(split_decoded[1].strip()[2:7])

                    self.pressure_raw_value = float(
                        split_decoded[2].strip()[2:11])

                    self.hum_raw_value = float(split_decoded[3].strip()[2:8])

                    self.accel_raw_value = float(split_decoded[4].strip()[2:7])

                    self.gyro_x_raw_value = float(
                        split_decoded[5].strip()[2:6])
                    self.gyro_y_raw_value = float(
                        split_decoded[5].strip()[7:12])

                    self.mag_x_raw_value = float(split_decoded[6].strip()[2:8])
                    self.mag_y_raw_value = float(
                        split_decoded[6].strip()[9:15])
                    self.mag_z_raw_value = float(
                        split_decoded[6].strip()[16:21])

                    # Calculations
                    self.pressure_value = (-0.012 *
                                           self.pressure_raw_value) + 1260
                    self.hum_value = (-2.25 *
                                      self.hum_raw_value) + 247.5
                    self.temp_value = (-2 *
                                       self.temp_raw_value) + 140
                    self.gyro_x_value = (self.gyro_x_raw_value * 10)
                    self.gyro_y_value = (self.gyro_y_raw_value * 10)
                    self.mag_x_value = (self.mag_x_raw_value * 50)
                    self.mag_y_value = (self.mag_y_raw_value * 50)
                    self.mag_z_value = (
                        self.mag_z_raw_value * 50)  # 200 / 4 = 50
                    self.accel_value = self.accel_raw_value * 20
                    self.fluxer_value = self.fluxer_raw_value * 20
                elif (self.mode == 4):
                    pass
                elif (self.mode == 5):
                    self.fluxer_raw_value = int(
                        split_decoded[1].strip()[9:10])
            except:
                print(
                    "I cannot convert string to float")


if __name__ == "__main__":

    initialise = Initialise()

    gyro_x = Gyro_X()
    gyro_y = Gyro_Y()
    mag_z = Mag_Z()
    mag_y = Mag_Y()
    mag_x = Mag_X()
    # speedometer = Speedometer()
    # speedometer.load_image()

    pressure = Pressure()
    humidity = Humidity()
    temperature = Temperature()
    accel = Accel()
    flux = Fluxer()
    warning = Warning()

    text = Text()

    updatevalues = UpdateValues()

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_f:
                    initialise.resolution = (1024, 600)
                    initialise.screen = pygame.display.set_mode(
                        initialise.resolution, pygame.FULLSCREEN)
                elif event.key == pygame.K_g:
                    initialise.resolution = (1024, 600)
                    initialise.screen = pygame.display.set_mode(
                        initialise.resolution, pygame.RESIZABLE)

        initialise.screen.fill(initialise.BLACK)

        # speedometer.load_image()
        # speedometer.draw_arc(updatevalues.speed_value)

        gyro_x.draw_rect(updatevalues.gyro_x_value)
        text.message_display(text="Gyroscope X axis : {}".format(
            updatevalues.gyro_x_raw_value), x_position=200,
            y_position=190)

        gyro_y.draw_rect(updatevalues.gyro_y_value)
        text.message_display(text="Gyroscope Y axis : {}".format(
            updatevalues.gyro_y_raw_value), x_position=200,
            y_position=90)

        mag_z.draw_rect(updatevalues.mag_z_value)
        text.message_display(text="Magnetometer Z axis : {:.2f}".format(
            updatevalues.mag_z_raw_value), x_position=500,
            y_position=90)

        mag_y.draw_rect(updatevalues.mag_y_value)
        text.message_display(text="Magnetometer Y axis : {:.2f}".format(
            updatevalues.mag_y_raw_value), x_position=500,
            y_position=190)

        mag_x.draw_rect(updatevalues.mag_x_value)
        text.message_display(text="Magnetometer X axis : {:.2f}".format(
            updatevalues.mag_x_raw_value), x_position=500,
            y_position=290)

        pressure.draw_arc(updatevalues.pressure_value)
        text.message_display(text="Pressure : {:.2f} (Pa)".format(
            updatevalues.pressure_raw_value), x_position=800,
            y_position=75)

        humidity.draw_arc(updatevalues.hum_value)
        text.message_display(text="Humidity : {:.2f} (%rh)".format(
            updatevalues.hum_raw_value), x_position=800,
            y_position=250)

        temperature.draw_arc(updatevalues.temp_value)
        text.message_display(text="Temperature : {:.2f} (deg C)".format(
            updatevalues.temp_raw_value), x_position=800,
            y_position=425)

        accel.draw_rect(updatevalues.accel_value)
        text.message_display(text="Accelerometer Z axis : {:.2f}".format(
            updatevalues.accel_raw_value), x_position=1100,
            y_position=75)

        flux.draw_rect(updatevalues.fluxer_value)
        text.message_display(text="Fluxer : {}".format(
            updatevalues.fluxer_raw_value), x_position=1050,
            y_position=175)

        if (updatevalues.mode == 1):
            text.title(text="Exploration Mode", x_position=450,
                       y_position=35)
        elif (updatevalues.mode == 2):
            text.title(text="Exploration Warning State", x_position=450,
                       y_position=35)
            warning.load_image()
        elif (updatevalues.mode == 3):
            text.title(text="Battle Mode", x_position=450,
                       y_position=35)
        elif (updatevalues.mode == 4):
            text.title(text="Battle Warning State", x_position=450,
                       y_position=35)
            warning.load_image()
        elif (updatevalues.mode == 5):
            text.message_display(text="Fluxer Fire", x_position=1050,
                                 y_position=400)

        updatevalues.transferValues()

        initialise.clock.tick(60)
        pygame.display.update()

pygame.quit()
quit()
