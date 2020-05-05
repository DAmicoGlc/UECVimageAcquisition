import os, sys, time, re, json, shutil
import numpy as np
import cv2
from unrealcv.automation import UE4Binary
from unrealcv.util import read_png, read_npy
from unrealcv import client

import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from tkinter import *
from PIL import Image, ImageTk

import tensorflow as tf

from datasets import imagenet
from nets import inception_v4
from preprocessing import inception_preprocessing

class Window(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)               
        self.master = master
        self.viewMode = StringVar()
        self.log = Text(self, height=10, width=30)

        # Prediction Results
        self.prediction = []
        for i in range (0, 4):
            self.prediction.append(Text(self, height=11, width=30))

        # Classifier
        self.classifier = ImageClassifier()

        master.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.init_window()

    def on_closing(self):
        self.master.destroy()
        self.quit()

    #Creation of init_window
    def init_window(self):

        # changing the title of our master widget      
        self.master.title("GUI")

        # allowing the widget to take the full space of the root window
        self.pack(fill=BOTH, expand=1)      

        #place LOG
        Label(self, text="LOG Display", fg="red").place(x=660, y=400)
        self.log.place(x=660, y=420)
        self.log.insert(END, "LOG: Client is running...\n")
        scroll = Scrollbar(self, command=self.log.yview)
        self.log.configure(yscrollcommand=scroll.set)

        # place prediction
        Label(self, text="Prediction FRONT Camera", fg="red").place(x=660, y=0)
        Label(self, text="Prediction WIDE ANGLE Camera", fg="red").place(x=910, y=0)
        Label(self, text="Prediction LEFT Camera", fg="red").place(x=660, y=200)
        Label(self, text="Prediction RIGHT Camera", fg="red").place(x=910, y=200)
        self.prediction[0].place(x=680, y=20)
        self.prediction[1].place(x=930, y=20)
        self.prediction[2].place(x=680, y=220)
        self.prediction[3].place(x=930, y=220)
        
        for i in range(0, 4):
            scroll = Scrollbar(self, command=self.log.yview)
            self.prediction[i].configure(yscrollcommand=scroll.set)

        # Viewmode radio button
        MODES = [
            ("Lit View Mode", "Lit"),
            ("Depth View Mode", "Depth"),
            ("Normal View Mode", "Normal")
        ]
        
        x=0
        y=480
        text = Label(self, text="View Mode", fg="red")
        text.place(x=x, y=y)
        for text, mode in MODES:
            y = y + 20
            b = Radiobutton(self, text=text,
                            variable=self.viewMode, value=mode)
            b.place(x=x, y=y)

        self.viewMode.set("Lit") # initialize

        # creating a button instance
        startCapturing = Button(self, text="CAPTURE", height=3, width=20, fg="green", command=self.imageCapture)
        # placing the button on my window
        startCapturing.place(x=400, y=520)

    def showImageCaptured(self, images):
        plt.close('all')
        figure, axarr = plt.subplots(2,2)
        axarr[0,0].imshow(images[0])
        axarr[0,1].imshow(images[1])
        axarr[1,0].imshow(images[2])
        axarr[1,1].imshow(images[3])
        canvas = FigureCanvasTkAgg(figure, self)
        canvas.get_tk_widget().place(x=0, y=0)

        text = Label(self, text="Image "+self.viewMode.get()+" from simulation!")
        text.place(x=0, y=0)

    def imageCapture(self):
        self.stop = False

        cmd = 'vget /cameraComponent/0/all/' + self.viewMode.get().lower() + ' npy'
        res = client.request(cmd)
        if not isinstance(res, str):
            images = read_npy(res)
            self.log.insert(END, "LOG: Camera component captured\n")
        else:
            images = []
            self.log.insert(END, "LOG ERROR: Can't parse npy -> " + str(res) + "\n")

        self.showImageCaptured(images)

        self.predict(images)

    def predict(self, images):
        for idx, img in enumerate(images):
            self.prediction[idx].delete('1.0', END)
                
            result = self.classifier.classify(img)
            self.prediction[idx].insert(END, result)

class ImageClassifier():
    def __init__(self):
        self.slim = tf.contrib.slim
        self.image_size = inception_v4.inception_v4.default_image_size
        self.checkpoints_dir = 'checkpoints'
        self.names = imagenet.create_readable_names_for_imagenet_labels()
        self.arg_scope = inception_v4.inception_v4_arg_scope()
        self.image = tf.placeholder(tf.uint8, [480, 640, 3])
        self.processed_image = inception_preprocessing.preprocess_image(self.image,
                                                                        self.image_size, self.image_size,
                                                                        is_training=False)
        self.processed_images = tf.expand_dims(self.processed_image, 0)

        # processed_images will be a 1x299x299x3 tensor of float32

        # Create the model, use the default arg scope to configure the batch norm parameters.
        with self.slim.arg_scope(self.arg_scope):
            self.logits, self.end_points = inception_v4.inception_v4(self.processed_images, num_classes=1001,
                                                                     is_training=False)
            self.probs = tf.nn.softmax(self.logits)

        self.init_fn = self.slim.assign_from_checkpoint_fn(
            os.path.join(self.checkpoints_dir, 'inception_v4.ckpt'),
            self.slim.get_model_variables('InceptionV4'))

        config = tf.compat.v1.ConfigProto()
        config.gpu_options.allow_growth = True
        self.session = tf.compat.v1.Session(config=config)
        self.init_fn(self.session)

    def classify(self, img):
        height, width = img.shape[:2]

        feed_dict = {self.image: img}
        probabilities = self.session.run(self.probs, feed_dict=feed_dict)
        probabilities = probabilities[0, 0:]
        sorted_inds = [i[0] for i in sorted(enumerate(-probabilities), key=lambda x: x[1])]

        response = ""

        for i in range(5):
            index = sorted_inds[i]
            response = response + "Probability {:.2f}".format(probabilities[index]*100) + "%\n[" + self.names[index] + "]\n"
        
        return response

def main():
    client.connect()
    if not client.isconnected():
        print("LOG: UnrealCV server is not running")
    else:
        root = Tk()

        #size of the window
        root.geometry("1200x610")

        app = Window(root)
        
        root.mainloop()

        client.disconnect()

if __name__ == "__main__":
    main()