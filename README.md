# Unreal-GPU-Driven-CubeIndirectDraw
用自定义SceneProxy，VertexFactory，PrimitiveComponent的方式，GPU-Driven绘制一排Cube，效果如下图所示
![CubeIndirectDraw](/CubeIndirect.gif)
通过用Renderdoc截帧能看到，绘制Cube的命令变成了Indirect的
![CubeIndirectDraw-renderdoc](/CubeIndirectDraw-renderdoc.png)
----
遇到不少坑，都记录在注释里了。
目前的实现不支持VSM，主要是因为还没支持PrimitiveIdStream，之后看有没时间加上
同时这种实现方式做不到剔除，最多视锥剔除，之后可能改成RDG的版本看看能不能用上HZB加上OC