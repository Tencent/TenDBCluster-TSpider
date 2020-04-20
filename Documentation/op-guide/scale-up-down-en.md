# TenDB Cluster Scale Up and Down
TenDB Cluster can flexibly scale up and down without affecting online service.
At present, the cluster supports horizontal expansion and contraction of the access layer TSpider, and vertical expansion and contraction of the storage layer TenDB.

- TSpider horizontal expansion and contraction
```
By adding or removing TSpider nodes, horizontally expand or shrink the TSpider nodes of the entire cluster to achieve the purpose of expansion and shrinkage
```

- TenDB vertical expansion and contraction
```
Through data relocation and master-slave switching, the existing TenDB hardware resources are replaced to achieve the purpose of storage expansion and contraction. Replace from low configuration to high configuration machine during capacity expansion, and replace from high configuration to low configuration machine during capacity reduction.
```

Regardless of whether it is  access layer or storage layer  ,the expansion and contraction  relies on the central control node Tdbctl to modify the route, which is achieved after global overall management.


[Proxy Layer Scaling](TSpider-scale-en.md)

[Data Layer Scaling](TenDB-scale-en.md)