Menu
1. Load New Data File
2. Show All Polygons
3. Select Polygon to be Displayed
4. 3D Translation
5. 3D Rotation
6. 3D Scaling
9. Reset “GluLookAt” View
0. Exit Program; Output Data into Input File (Alter Line 718 to Change Output Destination)

w. Increase gluLookAt y value by 0.1
s. Decrease gluLookAt y value by 0.1
d. Increase gluLookAt x value by 0.1
a. Decrease gluLookAt x value by 0.1
e. Increase gluLookAt z value by 0.1
q. Decrease gluLookAt z value by 0.1

I programmed this using Xcode in Mac OS. 

The format of the UI is the same in the sample files posted on Canvas, where the YZ-Plane is on the lower left-hand corner, XY-Plane is on the top-left corner, and XZ-Plane is on the top-right corner. Unfortunately, I was unable to separate each sub-window by lines. Also, the “3D Polygon” window only works on bigger screens. The window does not fit into my 13” laptop. If possible, please view on a bigger monitor.

I provided functionality to change the eye view by gluLookAt for fun, but please keep in mind it does not alter the actual point locations. 

You are able to show all polygons (2) or select and individual polygon to be displayed (3). When transforming, if all polygons are shown, the console will prompt you to for your selection of polygon. If only one polygon is shown, then it will skip the prompt and choose the one that is shown.


1. Load New Data File
	Press “1” in the “3D Polygon” window. 

	In the CONSOLE window, type the file name you would like to load.

2. Show All Polygons
	Press “2” in the “3D Polygon” window. 

3. Select Polygon to be Displayed
	Press “3” in the “3D Polygon” window.
	
	In the CONSOLE window:
	Enter from [0 - (numberOfPolygons - 1)] to select the polygon.

4. 3D Translation
	Press “4” in the “3D Polygon” window.
	
	In the CONSOLE window:
	May have to select the polygon depending on whether one or all polygons are shown.
	Enter the translation vector separated by spaces [x y z].

5. 3D Rotation
	Press “5” in the “3D Polygon” window.

	In the CONSOLE window:
	May have to select the polygon depending on whether one or all polygons are shown.
	Enter the first point for rotational axis separated by spaces [x y z].
	Enter the second point for rotational axis separated by spaces [x y z].
	Enter the rotation angle.

6. 3D Scaling 
	Press “6” in the “3D Polygon” window.

	In the CONSOLE window:
	May have to select the polygon depending on whether one or all polygons are shown.
	Enter the scale factor.

9. Reset “gluLookAt” View
	Press “9” in the “3D Polygon” window.
	
	This will reset the gluLookAt view by x = 0.0, y = 0.0, and z = 3.0

0. Exit Program; Output Data into Input File 
	Press “0” in the “3D Polygon” window.

	Alter Line 718 in the source code to change the output destination.

