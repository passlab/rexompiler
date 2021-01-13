
enum tgt_map_type {
  // No flags
  OMP_TGT_MAPTYPE_NONE            = 0x000,
  // copy data from host to device
  OMP_TGT_MAPTYPE_TO              = 0x001,
  // copy data from device to host
  OMP_TGT_MAPTYPE_FROM            = 0x002,
  // copy regardless of the reference count
  OMP_TGT_MAPTYPE_ALWAYS          = 0x004,
  // force unmapping of data
  OMP_TGT_MAPTYPE_DELETE          = 0x008,
  // map the pointer as well as the pointee
  OMP_TGT_MAPTYPE_PTR_AND_OBJ     = 0x010,
  // pass device base address to kernel
  OMP_TGT_MAPTYPE_TARGET_PARAM    = 0x020,
  // return base device address of mapped data
  OMP_TGT_MAPTYPE_RETURN_PARAM    = 0x040,
  // private variable - not mapped
  OMP_TGT_MAPTYPE_PRIVATE         = 0x080,
  // copy by value - not mapped
  OMP_TGT_MAPTYPE_LITERAL         = 0x100,
  // mapping is implicit
  OMP_TGT_MAPTYPE_IMPLICIT        = 0x200,
  // copy data to device
  OMP_TGT_MAPTYPE_CLOSE           = 0x400,
  // runtime error if not already allocated
  OMP_TGT_MAPTYPE_PRESENT         = 0x1000,
  // descriptor for non-contiguous target-update
  OMP_TGT_MAPTYPE_NON_CONTIG      = 0x100000000000,
  // member of struct, member given by [16 MSBs] - 1
  OMP_TGT_MAPTYPE_MEMBER_OF       = 0xffff000000000000
};
