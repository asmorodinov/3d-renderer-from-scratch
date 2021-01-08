# 3d-renderer-from-scratch
Basic software 3d renderer (hopefully crossplatform) with minimum dependencies on other libraries
For now the actual dependencies are the following:  
- SFML to render screen buffer on a window
- glm
- stb_image

![alt text](img.png)


TODO list:

Next up is: 
- [x] Clipping triangles against frustrum planes
- [x] There is something rotten in the state of Denmark... (fix strange projection bug)  
	Visual bug is "fixed" by setting near frustrum plane to a really small value  
	(this is a workaround, and not really a solution to the original problem, but this will work for now) 
- [ ] ...

Potential new features:
- [ ] Cubemap textures
- [ ] Different texture sampling modes
- [ ] Alpha blending
- [ ] Shadow mapping
- [ ] Different post-processing effects (e.g. FXAA)
- [ ] Mipmapping for textures
- [ ] ...

In general:
- [ ] improving perfomance
- [ ] cleaning up and refactoring code
- [ ] fixing bugs
- [ ] ...
