
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

typedef enum kmp_sched_t {
  kmp_sched_static_chunk = 33,
  kmp_sched_static_nochunk = 34,
  kmp_sched_dynamic = 35,
  kmp_sched_guided = 36,
  kmp_sched_runtime = 37,
  kmp_sched_auto = 38,

  kmp_sched_static_balanced_chunk = 45,

  kmp_sched_static_ordered = 65,
  kmp_sched_static_nochunk_ordered = 66,
  kmp_sched_dynamic_ordered = 67,
  kmp_sched_guided_ordered = 68,
  kmp_sched_runtime_ordered = 69,
  kmp_sched_auto_ordered = 70,

  kmp_sched_distr_static_chunk = 91,
  kmp_sched_distr_static_nochunk = 92,
  kmp_sched_distr_static_chunk_sched_static_chunkone = 93,

  kmp_sched_default = kmp_sched_static_nochunk,
  kmp_sched_unordered_first = kmp_sched_static_chunk,
  kmp_sched_unordered_last = kmp_sched_auto,
  kmp_sched_ordered_first = kmp_sched_static_ordered,
  kmp_sched_ordered_last = kmp_sched_auto_ordered,
  kmp_sched_distribute_first = kmp_sched_distr_static_chunk,
  kmp_sched_distribute_last =
      kmp_sched_distr_static_chunk_sched_static_chunkone,

  /* Support for OpenMP 4.5 monotonic and nonmonotonic schedule modifiers.
   * Since we need to distinguish the three possible cases (no modifier,
   * monotonic modifier, nonmonotonic modifier), we need separate bits for
   * each modifier. The absence of monotonic does not imply nonmonotonic,
   * especially since 4.5 says that the behaviour of the "no modifier" case
   * is implementation defined in 4.5, but will become "nonmonotonic" in 5.0.
   *
   * Since we're passing a full 32 bit value, we can use a couple of high
   * bits for these flags; out of paranoia we avoid the sign bit.
   *
   * These modifiers can be or-ed into non-static schedules by the compiler
   * to pass the additional information. They will be stripped early in the
   * processing in __kmp_dispatch_init when setting up schedules, so
   * most of the code won't ever see schedules with these bits set.
   */
  kmp_sched_modifier_monotonic = (1 << 29),
  /**< Set if the monotonic schedule modifier was present */
  kmp_sched_modifier_nonmonotonic = (1 << 30),
/**< Set if the nonmonotonic schedule modifier was present */

#define SCHEDULE_WITHOUT_MODIFIERS(s)                                          \
  (enum kmp_sched_t)(                                                          \
      (s) & ~(kmp_sched_modifier_nonmonotonic | kmp_sched_modifier_monotonic))
#define SCHEDULE_HAS_MONOTONIC(s) (((s)&kmp_sched_modifier_monotonic) != 0)
#define SCHEDULE_HAS_NONMONOTONIC(s)                                           \
  (((s)&kmp_sched_modifier_nonmonotonic) != 0)
#define SCHEDULE_HAS_NO_MODIFIERS(s)                                           \
  (((s) & (kmp_sched_modifier_nonmonotonic | kmp_sched_modifier_monotonic)) == \
   0)

} kmp_sched_t;
