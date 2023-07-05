from ultralytics.yolo.utils.plotting import Annotator
from ultralytics import YOLO
from datetime import datetime
import cv2
import json


# TODO: Replace print with logging.debug
# TODO: Check alternative to OpenCV for image saving to disk. (Assuming OpenCV is slow to load)


# Load a pretrained YOLO model (recommended for training)
#! WARNING: This could take a long time to load
# ? Use PyTorch to load the model?
model = YOLO('./vision/model/w_best.pt')


def infer(img: str):

    # result = model.predict(img, conf=0.5, show=True, save=True, save_txt=True, save_conf=True)
    result = model(img)[0]
    now = datetime.now()

    #! Could annotation be a bottleneck?
    # Visualize for debugging
    # https://stackoverflow.com/questions/75324341/yolov8-get-predicted-bounding-box
    annotator = Annotator(img)
    boxes = result.boxes

    for box in boxes:

        # get box coordinates in (top, left, bottom, right) format
        b = box.xyxy[0]
        c = box.cls
        annotator.box_label(b, model.names[int(c)])

    inference_frame = annotator.result()
    inference_win = 'YOLO V8 Inference Result'
    # Re-position Window
    # cv2.namedWindow(inference_win)
    # cv2.moveWindow(inference_win, 640, 0)
    # cv2.imshow(inference_win, inference_frame)
    # if cv2.waitKey(1) == ord('q'):
    #     cv2.destroyAllWindows()

    #! ---------------------------------------------------------

    # ? Trickier to implement than I thought -> Wheel rack!!!
    detected_object = ''

    # * Log Results
    # ? Could logging be threaded to further improve performance?
    if result:
        # * manually format json
        inference_result_json = result.tojson()
        inference_result_list = json.loads(inference_result_json)

        # Add date time information
        for object_dict in inference_result_list:
            object_dict["datetime"] = str(now)

        inference_result_json_object = json.dumps(
            inference_result_list, indent=4)

        # Parse JSON object <filter out unwanted data>
        filtered_json_list = []
        for object in inference_result_list:
            filtered_json_list.append(object["box"])

        with open(f"./vision/logs/labels/results.json", "a") as f:
            f.write(inference_result_json_object)
        print("[SUCCESS] Logged JSON Object to results.json")

        # If no prediction, this returns an empty list
        # Error handling to be carried forward.
        # save visualization
        # // result.save(f"./vision/logs/images/{now}.jpg")
        cv2.imwrite(
            f'./vision/logs/images/{now} {detected_object}.jpg', inference_frame)
        print(f"[{now}]> Save result as image [SUCCESS]")
        result.save_txt(f"./vision/logs/labels/results.txt")

        return filtered_json_list

    else:
        print(f"[{now}]> No prediction")
        # Save frame
        cv2.imwrite(f'./vision/logs/images/{now} None.jpg', inference_frame)
        print(f"[{now}]> Save None result as image [SUCCESS]")

        return []
