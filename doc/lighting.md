
Notes on Riot Engine lighting
=============================

Fixed layer light
-----------------
Layers have one fixed, directional light. It's direction is defined by an
azimuth (simply called "direction" in the editor) and an ascension angle.

Layer lights have a diffuse component and an ambient component that work as one
would expect from a Blinn-Phong model with the exception that the ambient component
seems to be multiplied by a factor of approx. 2 before being added.

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


Attenuation research
--------------------
Research of the used attenuation function is ongoing. Here are a few 
experimental samples, taken using a set of pure white models using a pure white
light. The 15 models (retextured gravestones) had one, flat face directly 
facing the light and were positioned away from the light with distance 
increasing in steps of 0.5lu.

The first object was at a distance of 0.5lu, the last one at a distance of
7.5lu. The light used had a radius of 7lu, so the second-to-last gravestone was
positioned exactly on the edge of the radius, while the last one stood outside
the lights radius.

Here is a vector containing the distances: 
(for convenient pasting into Octave/MATLAB)
```matlab
    [0.5 1.0 1.5 2.0 2.5 3.0 3.5 4.0 4.5 5.0 5.5 6.0 6.5 7.0 7.5]
```
    
The following are the sampled gravestone colors for a given intensity setting.
These are taken from the pure RGB values as rendered in windows mode by Drakan
running on Wine. Since the light and textures were pure white, the given value
occured in all channels.

Intensity=1.0
```matlab
    [253 249 243 234 223 209 192 172 150 126  99  69  37   2   0]
```

Intensity=2.0
```matlab
    [255 255 255 255 255 255 255 255 255 252 198 138  74   4   0]
```

Intensity=0.5
```matlab
    [126 124 121 117 111 104  96  86  75  63  49  34  18   1   0]
```

Intensity=0.1
```matlab
    [ 25  24  24  24  23  22  20  19  17  15  12   9   6   3   0]
```


Plotting these out, it is easily revealed that the curve with 1.0 intensity
can be warped into the others by multiplying with their respective intensity
and clamping the curve between 0 and 255. Thus it is apparent that the 
intensity is simply a linear factor in the color calculation.

This also means we can focus on the i=1.0-curve when replicating the
attenuation behaviour.

A simple, 2nd order least-squares approximation for the attenuation factor
(with d=distance/radius) is as follows:

    a = -0.82824*d^2 - 0.13095*d + 1.01358
    
    



