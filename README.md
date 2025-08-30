This project is a solution of well-known Marching Cubes problem of seamlessly stiching together triangles generated from voxel data at different resolutions, so LOD can be implenented for
Marching cubes. It is alternative for Transvoxels algorithm, but transitio cells are generated not instead of low resolution cells, but instead of high resolution cells.
It requires large triangulations table, so we provide an tool for generation this tables (TransitionCellsTriangulation project in this repository). 
