#!/usr/bin/python3

import cv2
import numpy as np

# Load YOLO
net = cv2.dnn.readNet("yolov3-tiny.weights", "yolov3-tiny.cfg")
output_layers = net.getUnconnectedOutLayersNames()

# Connect to the video stream
cap = cv2.VideoCapture("http://192.168.188.104:8080/video")
frame_id = 0

while True:
    _, frame = cap.read()
    frame_id += 1
    
    # Skip every other frame to increase speed
    if frame_id % 50 == 0:
        continue

    # Resize frame to reduce computation
    frame = cv2.resize(frame, (416, 416))
    
    # Perform detection
    blob = cv2.dnn.blobFromImage(frame, 0.00392, (416, 416), (0, 0, 0), True, crop=False)
    net.setInput(blob)
    outs = net.forward(output_layers)

    # Get the frame dimensions
    height, width, _ = frame.shape

    # Showing informations on the screen
    for out in outs:
        for detection in out:
            scores = detection[5:]
            class_id = np.argmax(scores)
            confidence = scores[class_id]
            
            if confidence > 0.5: # Confidence threshold
                # Object detected
                center_x = int(detection[0] * width)
                center_y = int(detection[1] * height)
                w = int(detection[2] * width)
                h = int(detection[3] * height)
                
                # Rectangle coordinates
                x = int(center_x - w / 2)
                y = int(center_y - h / 2)
                
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

                # Extract color information
                roi = frame[y:y+h, x:x+w]  # Extract ROI from image
                hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)  # Convert ROI to HSV color space

                # Calculate average color within ROI
                average_color_per_row = np.average(hsv, axis=0)
                average_color = np.average(average_color_per_row, axis=0)
                print(f"Average color in BGR space: {average_color}")  # Print average color

    # Show the frame with detected objects
    cv2.imshow("Image", frame)

    # Break the loop if 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
