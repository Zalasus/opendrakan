
Notes on Riot Engine lighting
=============================

Fixed layer light
-----------------
Layers have one fixed, directional light. It's direction is defined by an
azimuth (simply called "direction" in the editor) and an ascension angle.

Layer lights have a diffuse component and an ambient component that work as one
would expect from a Blinn-Phong model.

When the vertices of two or more layers with the same XZ coordinate are within
±2wu of each other on the Y axis, the lighting properties of those layers are
blended for each of the vertices within that range.

The above highly suggests that the engine does not use the GPU's light state
for layer lights, but instead bakes it into the vertex colors of each layer.
At least, the latter would be the obvious choice for OpenDrakan to do, as this
save us one light state and makes the blending of layer lights as easy as pie.
This only would come at the cost of one more vertex attribute array.


Light objects
-------------
By using the right RFL class, level objects can represent point lights. These
affect layers as well as other objects.

The amount of light objects that can influence a layer simultaneously is
limited. This number is much smaller than the 8 lights one would expect from 
contemporary hardware and also seems to vary depending on the position of those
objects. Right now, the biggest number I observed was two light objects at the
same time.

I don't know whether light objects can move or not.


Object lighting
---------------
Normal objects are influenced by both layer lights and light objects.

Which layer light is to be applied to an object is determined by finding the
first layer directly below the object that does not have a hole (null texture)
at that location.

However, some objects define a specific layer to be used as a light source, 
overriding the one below the object.

I don't know how many or which exact point lights affect an object, but it
would seem like a sensible choice to pick the closest lights which's radius
contains the object to be lighted.







