# COMPLETELY FORKED FROM https://github.com/unhappyworld/rbxRenderHook.
# 2012-renderer  
An open-source place renderer for the version 0.69.0.646 Roblox Studio version (compatible with later versions, this was for December 2012 originally)  
You can get this client here: https://archive.org/details/roblox-studio_20250323  

- On this version, "band aid ass fix" from watrabi was added which allows everything to load, preventing place renders from rendering without having most of it's bricks.
- Image uploading has been added back, which was removed in unhappyworld's fork of rbxRenderHook. However image uploads happen from the DLL and not from the lua command you execute in the client. Rendering will also close the client automatically to make it easier for renderer arbiters to mark the job done.

## Usage  
Compile the project in **DEBUG** mode (Release mode could cause issues)  
Open the **MFCStudio** (RobloxApp clients are not compatible, no idea why)  
Run the **render** function (read functions below)

## Functions  
`render(filepath, x, y, isCharacter)`  
Example:  
`render("TestRender.png", 1280, 720, 1)`  
1 is true, 0 is false. This will toggle whether or not Thumbnail View will happen.
  
## Troubleshooting
1. Help! It's crashing!  
  - Make sure you compiled in **DEBUG** mode and that you're using **OPENGL**

2. I have no sky in my renders!
  - Make sure you are using **OPENGL**. **Direct3D** makes the sky invisible.  
  
## FAQ
1. Can you hide the sky?
  - **Yes! You can.** Just change from OpenGL to Direct3D in studio settings, and also change **ViewBase::CreateView** to **Direct3D9** instead of **OpenGL**  
  
2. How can I use this for my revival
   - **Make your own renderer arbiter that launches the client and executes render scripts.** Configure the domain links to upload the rendered image to your own website. You will also need to have two studio clients using different GlobalSettings13.xml files. An easy way to do this is to edit the secondary client in HXD and change the name of the config file Example: "GlobalSettings##.xml". Then, on your two clients, change render settings to be Direct3d and OpenGL. Make sure your arbiter launches two different clients depending on what it's rendering.
