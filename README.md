This plugin is designed to be used along with Voxel Plugin 2.0

## Notes
Make sure to set these settings on your textures you want to load as a heightmap

![compression](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/0b6c5b6d-7b95-44b0-8bec-0308fcd4ec9c)

![lod](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/f1e8aead-754b-4369-a0d7-29343ac90baf)

Additionally, if you want the best results make sure it's G16.

I have a Box Blur for G8 to remove the banding *which is due to the lower pixel depth*

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/18fac62b-fa2b-4f7c-955e-198ae42ec3a5)

Here's an example of what an 8-bit *(G8)* texture would look like with the banding

*this is greatly reduced with the Box Blur I have on G8*

8-bit, pre Box Blur

![before](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/df460125-5882-400a-a0a9-1f4bc84800ac)

8-bit, post Box Blur

![after](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/e138bbb5-42bd-4035-b738-46cec6ef3306)

## Sample Heightmap Brush
![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/6858c6ca-d7da-4cec-82cf-cd01cc92bb45)

Located at:

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/d9a77473-e927-4758-954f-0dee50f0f86b)

## Nodes

### Heightmap Texture
Make Heightmap From Texture

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/3335b394-5f35-40cd-a3fa-4c361c08284d)

Get Max Height from Heightmap

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/a1b4f817-d7b7-4fbd-8ea9-50f1b16a7b18)

Get Min Height from Heightmap

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/26b94842-d44d-4408-8d3d-3a8670d8f35c)

Get Width from Heightmap Texture

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/f3d78966-5177-4b8b-acf7-9c6ad2b1b7fc)

Sample Heightmap From Texture

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/d63d705e-3d27-4176-9343-ba868fc5637b)

### Map
Make MAP

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/d35dbb34-a713-45f9-b7d4-aaaf3f89e60d)

Find MAP Value

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/d6994446-44c3-44c0-be4a-9677012f5636)

Get MAP Keys

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/0822dd26-9c96-4b7f-889a-51639a45e3c0)

Get MAP Values

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/f9ba218e-b54e-41fb-a9c7-422c3e715595)

Get MAP Length

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/1233680f-79be-45ff-bb2f-7d4bb36390c8)

### Arrays
Array Get from Buffered Index

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/1cce12cb-3204-4d19-93ac-05509d8c83e5)

Array Set from Uniform Index & Value

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/393e6601-25c7-403a-9a5d-5ea3810cbd39)

### Noise
3D True Distance Noise

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/7ec67c8e-01eb-4086-aa7d-5457251d92ac)

2D IQ Noise

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/950c1385-8bd9-44bf-a751-230736321054)

2D Simplex Noise

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/0e108525-f162-48ed-b0e8-74f3ab2ede73)

2D Simplex Noise Fractal

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/b3a2a8ec-2df0-42bb-ae56-64c14db41aa9)

