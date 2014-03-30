PrincipalAxisComparator
=======================

This project was created for the final project of the software engineering subject from the VIBOT masters of computer vision and robotics.

The aim of the project is to implement different algorithms to compute the axes of a mesh and evaluate how robust they are against noise and deformations from an original mesh.

To do so it was found convenient to compare the alignments visually. A viewer that can display two methods next to each other was implemented to compare two methods easily.

![Application screenshot](http://i.imgur.com/zVMJAoP.jpg)

Dependencies
-----------
- Qt
- OpenGL
- OpenMesh

Install
-------

	mkdir build
	cd build
	qmake ../src
	make
	./visor

Principal axis computing methods implemented
--------------------------------------------

- PCA
- Y.S. Liu and K. Ramani. Robust principal axes determination for point-based shapes using least median of squares.(http://www.ncbi.nlm.nih.gov/pmc/articles/PMC2710849/)

	
Adding a principal axis computing method
----------------------------------------

For adding a new method, modify the class PrincipalAxisGenerator. This class has an enum that contains the possible methods. Add the new method there.
Modify the .ui of the main window in order to add another tab to each viewer in order to be able to display the method.
After that, modify MainWindow::on_computeButton_clicked(). In that method there is a variable called methods2compute. Push back the enum option of the new method.
Note that the order matters, so the last method pushed back will correspond to the right most tab of the viewer.

Then, on the function PrincipalAxisGenerator::compute() add another switch case that will compute the axis.
When compute() is called the attribute vertexes contains the input list of vertex of the mesh.

The following attributes of the class have to be filled with the proper data:
-center: Origin of the axis of the mesh
-axis1: X axis of the mesh
-axis2: Y axis of the mesh
-axis3: Z axis of the mesh

Authors of the project
----------------------

- Quim Sanchez
- Gerard Bahi

Demo
----

https://www.youtube.com/watch?v=DhJbje2yzvg
https://www.youtube.com/watch?v=V8jZxDHGpZQ
https://www.youtube.com/watch?v=lrotzWoZglg
