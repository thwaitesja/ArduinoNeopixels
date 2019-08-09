from PyQt5 import QtCore, QtWidgets, QtGui
import numpy as np
import serial
import serial.tools.list_ports as serports
import glob
import sys
import time

class ColorCircle(QtWidgets.QWidget):

    def __init__(self, parent):
        super().__init__()
        self.parent = parent
        self.radius = 100.
        self.setFixedSize(200, 200)
        self.setMouseTracking(True)
        self.color = QtGui.QColor(0, 0, 0, 0)


    def paintEvent(self, ev):
        super().paintEvent(ev)
        p = QtGui.QPainter(self)
        for i in range(self.width()):
            for j in range(self.height()):
                color = QtGui.QColor(255, 255, 255, 0)
                h = (np.arctan2(i-self.radius, j-self.radius)+np.pi)/(2.*np.pi)
                s = np.sqrt(np.power(i-self.radius, 2)+np.power(j-self.radius, 2))/self.radius
                v = 1.0
                if s < 1.0:
                    color.setHsvF(h, s, v, 1.0)
                    p.setPen(color)
                    p.drawPoint(i, j)
                else:
                    p.setPen(self.color)
                    p.drawPoint(i, j)


    def mousePressEvent(self, event):
        color = QtGui.QColor(255, 255, 255, 0)
        x = event.pos().x()
        y = event.pos().y()
        h = (np.arctan2(x - self.radius, y - self.radius) + np.pi) / (2. * np.pi)
        s = np.sqrt(np.power(x - self.radius, 2) + np.power(y - self.radius, 2)) / self.radius
        v = 1.0
        if s < 1.0:
            color.setHsvF(h, s, v, 1.0)
            self.color = color
            self.update()
            self.parent.colorupdate()


class communication():
    def __init__(self):
        self.ser = serial.Serial()
        #self.ser.baudrate = 38400
        self.ser.baudrate = 9600

    def connect(self, port):
        #self.ser.baudrate = 115200
        self.ser.port = port
        self.ser.open()

    def disconnect(self):
        self.ser.close()

    def check(self):
        return self.ser.is_open

    def serial_ports(self):
        return [port[0] for port in list(serports.comports())]

    def old_serial_ports(self):
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(20)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')
        result = []
        for port in ports:
            #print(i)
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result


    def updatemode(self, mode):
        self.ser.write(f'm{mode}n'.encode())
        self.ser.write(f'jn'.encode())

    def updatered(self, r):
        r = (3 - len(str(r))) * '0' + str(r)
        self.ser.write(f'r{r}n'.encode())

    def updateblue(self, b):
        b = (3 - len(str(b))) * '0' + str(b)
        self.ser.write(f'b{b}n'.encode())

    def updategreen(self, g):
        g = (3 - len(str(g))) * '0' + str(g)
        self.ser.write(f'g{g}n'.encode())

    def updatespeed(self, s):
        self.ser.write(f's{s}n'.encode())

    def sendmsg(self, r, g, b, speed, mode):
        #self.ser.write('0'.encode())
        #time.sleep(.1)
        r= (3-len(str(r)))*'0'+str(r)
        self.ser.write(f'r{r}n'.encode())
        #self.ser.write(f'{r},{g},{b},{speed},{mode}n'.encode())

    def readmsg(self):
        time.sleep(.1)
        print(self.ser.read_all())


class MainWindow(QtWidgets.QWidget):
    switch_window = QtCore.pyqtSignal()# perameter passing
    def __init__(self, parent):
        QtWidgets.QWidget.__init__(self)
        self.blue = parent.blue
        self.colorSpace = ColorCircle(self)
        selection =['Off', 'Solid', 'Fade', 'Flow', 'Chase', 'Random']
        self.select = QtWidgets.QComboBox()
        self.select.addItems(selection)
        self.select.currentIndexChanged.connect(self.state)
        self.dbutton = QtWidgets.QPushButton('Disconnect Light Strip')
        self.ubutton = QtWidgets.QPushButton('Update Lights')
        self.ubutton.clicked.connect(self.colorupdate)
        self.dbutton.clicked.connect(self.switch)
        # self.slabel = QtWidgets.QLabel('Speed').
        self.speed = QtWidgets.QSlider()
        self.speed.sliderMoved.connect(self.myupdate)
        self.speed.hide()
        self.UInit()

    def UInit(self):
        mypalette = self.palette()
        mypalette.setColor(self.backgroundRole(), QtCore.Qt.black)
        self.setPalette(mypalette)
        self.setWindowTitle("MYLED")
        #self.setWindowIcon(QtGui.QIcon('customIcon.png'))
        layout = QtWidgets.QGridLayout()
        layout.addWidget(self.dbutton, 0,0)
        #layout.addWidget(self.ubutton, 1,0)
        layout.addWidget(self.select, 2,0)
        # layout.addWidget(self.slabel)
        layout.addWidget(self.speed, 3,1)
        layout.addWidget(self.colorSpace, 3,0)
        # layout.addWidget(self.slide)
        # self.button.setPalette(self.mypalette)
        # self.setPalette(self.mypalette)
        self.setLayout(layout)

    def colorupdate(self):
        mycolor = self.colorSpace.color.getRgb()[0:3]
        self.blue.updatered(mycolor[0])
        time.sleep(.2)
        self.blue.updategreen(mycolor[1])
        time.sleep(.2)
        self.blue.updateblue(mycolor[2])
        self.blue.readmsg()

    def myupdate(self):
        self.blue.updatespeed(self.speed.value()//10)
        self.blue.readmsg()

    def state(self):
        self.blue.updatemode(self.select.currentIndex())
        self.blue.readmsg()
        if self.select.currentIndex() < 2:
            self.speed.hide()
        else:
            self.speed.show()

    def switch(self):
        self.blue.disconnect()
        self.switch_window.emit()


class LoginWindow(QtWidgets.QWidget):
    switch_window = QtCore.pyqtSignal()
    def __init__(self, parent):
        QtWidgets.QWidget.__init__(self)
        self.blue = parent.blue
        self.setWindowTitle("Connect to light Strip")
        self.layout = QtWidgets.QVBoxLayout()
        self.refresh()
        self.setLayout(self.layout)


    def refresh(self):
        self.clearLayout()
        self.button = QtWidgets.QPushButton('refresh')
        self.button.clicked.connect(self.refresh)
        self.layout.addWidget(self.button)
        for port in self.blue.serial_ports():
            self.layout.addWidget(self.make_button(port))


    def clearLayout(self):
        while self.layout.count():
            child = self.layout.takeAt(0)
            if child.widget():
                child.widget().deleteLater()


    def make_button(self, port):
        button = QtWidgets.QPushButton(port)
        button.clicked.connect(lambda: self.login(port))
        return button


    def login(self, port):
        self.blue.connect(port)
        if self.blue.check():
            self.switch_window.emit()


class Controller:
    def __init__(self):
        self.blue = communication()
        # print(serial_ports())
        self.main = 0

    def show_login(self):
        self.login = LoginWindow(self)
        self.login.switch_window.connect(self.show_main)
        self.login.show()
        if self.main:
            self.main.hide()


    def show_main(self):
        self.main = MainWindow(self)
        self.main.switch_window.connect(self.show_login)
        self.main.show()
        self.login.hide()







app =QtWidgets.QApplication(sys.argv)
controller = Controller()
controller.show_login()
sys.exit(app.exec_())


