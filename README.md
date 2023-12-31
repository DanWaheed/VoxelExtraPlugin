This plugin is designed to be used along with Voxel Plugin 2.0

## Notes
Make sure to set these settings on your textures you want to load as a heightmap

![compression](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/0b6c5b6d-7b95-44b0-8bec-0308fcd4ec9c)

![lod](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/f1e8aead-754b-4369-a0d7-29343ac90baf)

Additionally, if you want the best results make sure it's G16.

I have a Box Blur for G8 to remove the banding which results do to the lower pixel depth.

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/18fac62b-fa2b-4f7c-955e-198ae42ec3a5)

Here's an example of what an 8-bit *(G8)* texture would look like with the banding

*this is greatly reduced with the Box Blur I have on G8*

8-bit, pre Box Blur

<img width="1482" alt="bandy" src="https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/2eb8a04e-e8ee-4528-a65b-73e4c4785085">



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

### Arrays
Array Get from Buffered Index

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/1cce12cb-3204-4d19-93ac-05509d8c83e5)

### Noise
3D True Distance Noise

![image](https://github.com/DanWaheed/VoxelExtraPlugin/assets/117957636/7ec67c8e-01eb-4086-aa7d-5457251d92ac)
