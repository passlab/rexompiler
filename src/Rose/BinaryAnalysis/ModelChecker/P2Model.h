#ifndef ROSE_BinaryAnalysis_ModelChecker_P2Model_H
#define ROSE_BinaryAnalysis_ModelChecker_P2Model_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Stack.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/SymbolicSemantics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Model checking model using Partitioner.
 *
 *  This model checker uses @ref Rose::BinaryAnalysis::Partitioner2 data structures and is based on the
 *  model defined at one time by @ref Rose::BinaryAnalysis::FeasiblePath, which this model is meant to replace.
 *
 *  This model uses the symbolic semantics domain. Registers are stored in a generic register state that adapts to the
 *  registers that are in use. Memory can be list- or map-based and could possibly support purely symbolic memory as well. The
 *  list- and map-based memory states can also use concrete state from a @ref MemoryMap. */
namespace P2Model {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings for this model checker
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Settings for this model. */
struct Settings {
    /** Type of memory state to use. */
    enum class MemoryType {
        LIST,                                           /**< Reverse chronological list. This is more accurate. */
        MAP                                             /**< Map indexed by symbolic address hash. This is faster. */
    };

    TestMode nullRead = TestMode::MUST;                 /**< How to test for reads from the null page. */
    TestMode nullWrite = TestMode::MUST;                /**< How to test for writes from the null page. */
    TestMode oobRead = TestMode::OFF;                   /**< How to test for out-of-bounds reads. */
    TestMode oobWrite = TestMode::OFF;                  /**< How to test for out-of-bounds writes. */
    TestMode uninitVar = TestMode::OFF;                 /**< How to test for uninitialized variable reads. */
    rose_addr_t maxNullAddress = 4095;                  /**< Maximum address of the null page. */
    bool debugNull = false;                             /**< When debugging is enabled, show null pointer checking? */
    Sawyer::Optional<rose_addr_t> initialStackVa;       /**< Address for initial stack pointer. */
    MemoryType memoryType = MemoryType::MAP;            /**< Type of memory state. */
    bool solverMemoization = false;                     /**< Whether the SMT solver should use memoization. */
    bool traceSemantics = false;                        /**< Whether to trace all RISC operators. */
};

class SemanticCallbacks;

/** Command-line switches for settings.
 *
 *  Returns a description of command-line switches that can be used to initialize the specified settings object. */
Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function call stack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Describes one funtion call in the call stack. */
class FunctionCall {
    Partitioner2::FunctionPtr function_;
    rose_addr_t initialStackPointer_ = 0;
    rose_addr_t framePointerDelta_ = (rose_addr_t)(-4); // Normal frame pointer w.r.t. initial stack pointer
    Sawyer::Optional<rose_addr_t> returnAddress_;
    Variables::StackVariables stackVariables_;

public:
    ~FunctionCall();
    FunctionCall(const Partitioner2::FunctionPtr&, rose_addr_t initialSp, Sawyer::Optional<rose_addr_t> returnAddress,
                 const Variables::StackVariables&);

    /** Property: The function called. */
    Partitioner2::FunctionPtr function() const;

    /** Property: Initial stack pointer.
     *
     *  The concrete value of the stack pointer at the start of the function. */
    rose_addr_t initialStackPointer() const;

    /** Property: Stack variables. */
    const Variables::StackVariables& stackVariables() const;

    /** Property: Frame pointer delta.
     *
     *  Amount to add to the initial stack pointer to get the usual frame pointer.
     *
     * @{ */
    rose_addr_t framePointerDelta() const;
    void framePointerDelta(rose_addr_t);
    /** @} */

    /** Computed frame pointer.
     *
     *  This is the frame pointer computed from the initial stack pointer and the frame pointer delta. */
    rose_addr_t framePointer(size_t nBits) const;

    /** Address to which function returns
     *
     * @{ */
    Sawyer::Optional<rose_addr_t> returnAddress() const;
    void returnAddress(Sawyer::Optional<rose_addr_t>);
    /** @} */

    /** Some basic info about the function call. */
    std::string printableName(size_t nBits) const;
};

/** Function call stack.
 *
 *  A stack of functions that are in progress, with the most recent function call at the top of the stack. */
using FunctionCallStack = Sawyer::Container::Stack<FunctionCall>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics value
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Symbolic values with memory regions.
 *
 *  Values are symbolic, and some values also have memory region information. */
class SValue: public InstructionSemantics2::SymbolicSemantics::SValue {
public:
    /** Base class. */
    using Super = InstructionSemantics2::SymbolicSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

private:
    AddressInterval region_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(region_);
    }
#endif

public:
    ~SValue();

protected:
    SValue();

    SValue(size_t nBits, uint64_t number);

    explicit SValue(const SymbolicExpr::Ptr&);

public:
    /** Instantiate a new prototypical value.
     *
     *  Prototypical values are only used for their virtual constructors. */
    static Ptr instance();

    /** Instantiate a new data-flow bottom value. */
    static Ptr instanceBottom(size_t nBits);

    /** Instantiate a new undefined value. */
    static Ptr instanceUndefined(size_t nBits);

    /** Instantiate a new unspecified value. */
    static Ptr instanceUnspecified(size_t nBits);

    /** Instantiate a new concrete value. */
    static Ptr instanceInteger(size_t nBits, uint64_t value);

    /** Instantiate a new symbolic value. */
    static Ptr instanceSymbolic(const SymbolicExpr::Ptr &value);

public:
    virtual InstructionSemantics2::BaseSemantics::SValuePtr bottom_(size_t nBits) const override {
        return instanceBottom(nBits);
    }

    virtual InstructionSemantics2::BaseSemantics::SValuePtr undefined_(size_t nBits) const override {
        return instanceUndefined(nBits);
    }

    virtual InstructionSemantics2::BaseSemantics::SValuePtr unspecified_(size_t nBits) const override {
        return instanceUnspecified(nBits);
    }

    virtual InstructionSemantics2::BaseSemantics::SValuePtr number_(size_t nBits, uint64_t value) const override {
        return instanceInteger(nBits, value);
    }

    virtual InstructionSemantics2::BaseSemantics::SValuePtr boolean_(bool value) const override {
        return instanceInteger(1, value ? 1 : 0);
    }

    virtual InstructionSemantics2::BaseSemantics::SValuePtr copy(size_t newNBits = 0) const override;

    virtual Sawyer::Optional<InstructionSemantics2::BaseSemantics::SValuePtr>
    createOptionalMerge(const InstructionSemantics2::BaseSemantics::SValuePtr &other,
                        const InstructionSemantics2::BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const override;

public:
    /** Promote a base value to a MemoryRegionSemantics value.
     *
     *  The value @p v must have a MemoryRegionSemantics::SValue dynamic type. */
    static Ptr promote(const InstructionSemantics2::BaseSemantics::SValuePtr &v) { // hot
        Ptr retval = v.dynamicCast<SValue>();
        ASSERT_not_null(retval);
        return retval;
    }

public:
    /** Property: Optional memory region.
     *
     *  This property describes a contiguous region of memory into which this value points if this value were to be
     *  treated as a pointer.  The region usually corresponds to a variable in the source language.
     *
     *  @{ */
    AddressInterval region() const;
    void region(const AddressInterval&);
    /** @} */

public:
    virtual void print(std::ostream&, InstructionSemantics2::BaseSemantics::Formatter&) const override;
    virtual void hash(Combinatorics::Hasher&) const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for semantic state. */
using StatePtr = boost::shared_ptr<class State>;

/** Semantic state. */
class State: public InstructionSemantics2::SymbolicSemantics::State {
public:
    /** Base type. */
    using Super = InstructionSemantics2::SymbolicSemantics::State;

    /** Shared-ownership pointer. */
    using Ptr = StatePtr;

private:
    FunctionCallStack callStack_;

protected:
    State();
    State(const InstructionSemantics2::BaseSemantics::RegisterStatePtr&,
          const InstructionSemantics2::BaseSemantics::MemoryStatePtr&);

    // Deep copy
    State(const State&);

public:
    /** Allocating constructor. */
    static Ptr instance(const InstructionSemantics2::BaseSemantics::RegisterStatePtr&,
                        const InstructionSemantics2::BaseSemantics::MemoryStatePtr&);

    /** Deep-copy allocating constructor. */
    static Ptr instance(const StatePtr&);

    /** Virtual constructor. */
    virtual InstructionSemantics2::BaseSemantics::StatePtr
    create(const InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
           const InstructionSemantics2::BaseSemantics::MemoryStatePtr &memory) const override {
        return instance(registers, memory);
    }

    /** Virtual copy constructor. */
    virtual InstructionSemantics2::BaseSemantics::StatePtr clone() const override;

    /** Checked dynamic cast. */
    static Ptr promote(const InstructionSemantics2::BaseSemantics::StatePtr&);

public:
    /** Function call stack.
     *
     *  The stack of functions whose calls are in progress. The top of the stack is the most recent function call.
     *
     * @{ */
    const FunctionCallStack& callStack() const;
    FunctionCallStack& callStack();
    /** @} */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics domain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** RISC operators for model checking. */
class RiscOperators: public InstructionSemantics2::SymbolicSemantics::RiscOperators {
public:
    using Super = InstructionSemantics2::SymbolicSemantics::RiscOperators;

    using Ptr = RiscOperatorsPtr;

private:
    const Settings settings_;
    const Partitioner2::Partitioner &partitioner_;
    SmtSolver::Ptr modelCheckerSolver_;                 // solver used for model checking, different than RISC operators
    size_t nInstructions_ = 0;
    SemanticCallbacks *semantics_ = nullptr;
    AddressInterval stackLimits_;                       // where the stack can exist in memory
    bool computeMemoryRegions_ = false;                 // compute memory regions. This is needed for OOB analysis.

    mutable SAWYER_THREAD_TRAITS::Mutex variableFinderMutex_; // protects the following data m
    Variables::VariableFinderPtr variableFinder_unsync;       // shared by all RiscOperator objects

protected:
    RiscOperators(const Settings&, const Partitioner2::Partitioner&, ModelChecker::SemanticCallbacks*,
                  const InstructionSemantics2::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&,
                  const Variables::VariableFinderPtr&);

public: // Standard public construction-like functions
    ~RiscOperators();

    static Ptr instance(const Settings&, const Partitioner2::Partitioner&, ModelChecker::SemanticCallbacks*,
                        const InstructionSemantics2::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&,
                        const Variables::VariableFinderPtr&);

    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::SValuePtr&, const SmtSolverPtr&) const override;

    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::StatePtr&, const SmtSolverPtr&) const override;

    static Ptr promote(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &x);

public: // Supporting functions
    /** Property: Partitioner.
     *
     *  The partitioner specified when this object was constructed. */
    const Partitioner2::Partitioner& partitioner() const;

    /** Property: Model checker SMT solver.
     *
     *  This property holds the solver used for model checking, which can be different or the same as the solver used generally
     *  by this object's RISC operators.
     *
     * @{ */
    SmtSolver::Ptr modelCheckerSolver() const;
    void modelCheckerSolver(const SmtSolver::Ptr&);
    /** @} */

    /** Property: Compute memory regions for variables.
     *
     *  If true, then memory regions are computed and used. Otherwise the SValue memory regions are always empty.
     *
     * @{ */
    bool computeMemoryRegions() const;
    void computeMemoryRegions(bool);
    /** @} */

    /** Test whether the specified address is considered to be null.
     *
     *  An address need not be zero in order to be null. For instance, on Linux the entire first page of memory is generally
     *  unmapped in order to increase the chance that a null pointer to a struct/class/array still causes a segmentation fault
     *  even when accessing a member other than the first member.
     *
     *  If a null dereference is detected, then a NullDerefTag is thrown. */
    void checkNullAccess(const InstructionSemantics2::BaseSemantics::SValuePtr &addr, TestMode, IoMode);

    /** Test whether the specified address is out of bounds for variables.
     *
     *  If an OOB access is detected, then an OobTag is thrown. */
    void checkOobAccess(const InstructionSemantics2::BaseSemantics::SValuePtr &addr, TestMode, IoMode, size_t nBytes);

    /** Test whether the specified address accesses an uninitialized variable.
     *
     *  If an uninitialized access is detected, then an UninitReadTag is thrown. */
    void checkUninitVar(const InstructionSemantics2::BaseSemantics::SValuePtr &addr, TestMode, size_t nBytes);

    /** Property: Number of instructions executed.
     *
     *  This property contains teh number of instructions executed. It is incremented automatically at the end of each
     *  instruction, but the user can reset it to any other value.
     *
     * @{ */
    size_t nInstructions() const;
    void nInstructions(size_t);
    /** @} */

    /** Ensure call stack has a root function. */
    void maybeInitCallStack(rose_addr_t insnVa);

    /** Push a function onto the call stack. */
    void pushCallStack(const Partitioner2::FunctionPtr &callee, rose_addr_t initialSp, Sawyer::Optional<rose_addr_t> returnVa);

    /** Pop a function from the call stack.
     *
     *  This pops the function from the top of the call stack and optionally discards memory that is beyond the top of the
     *  stack. */
    void popCallStack();

    /** Remove old call stack entries.
     *
     *  Look at the current stack pointer and remove those function call entries that are beyond. In effect, this cleans up
     *  those functions that have returned.
     *
     *  Returns the number of items poppped from the call stack. */
    size_t pruneCallStack();

    /** Print information about the function call stack. */
    void printCallStack(std::ostream&, const std::string &prefix);

    /** Assign a region to an expression.
     *
     *  The region is assigned to the first argument, which is also returned.
     *
     *  If the first argument already has a region, then nothing is done.
     *
     *  Otherwise, if additional arguments are specified and exactly one has a region, then that region is assigned to the
     *  result.
     *
     *  Otherwise, we scan the function call stack (stored in the current semantic state) and look for a variable whose
     *  addresses include the result value. If such a variable is found, then its memory region is assigned to the result
     *  expression.
     *
     * @{ */
    InstructionSemantics2::BaseSemantics::SValuePtr
    assignRegion(const InstructionSemantics2::BaseSemantics::SValuePtr &result);

    InstructionSemantics2::BaseSemantics::SValuePtr
    assignRegion(const InstructionSemantics2::BaseSemantics::SValuePtr &result,
                 const InstructionSemantics2::BaseSemantics::SValuePtr &a);

    InstructionSemantics2::BaseSemantics::SValuePtr
    assignRegion(const InstructionSemantics2::BaseSemantics::SValuePtr &result,
                 const InstructionSemantics2::BaseSemantics::SValuePtr &a,
                 const InstructionSemantics2::BaseSemantics::SValuePtr &b);
    /** @} */

public: // Override RISC operations
    virtual void finishInstruction(SgAsmInstruction*) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    number_(size_t nBits, uint64_t value) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    extract(const InstructionSemantics2::BaseSemantics::SValuePtr&, size_t begin, size_t end) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    concat(const InstructionSemantics2::BaseSemantics::SValuePtr &lowBits,
           const InstructionSemantics2::BaseSemantics::SValuePtr &highBits) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    shiftLeft(const InstructionSemantics2::BaseSemantics::SValuePtr &a,
              const InstructionSemantics2::BaseSemantics::SValuePtr &nBits) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    shiftRight(const InstructionSemantics2::BaseSemantics::SValuePtr &a,
               const InstructionSemantics2::BaseSemantics::SValuePtr &nBits) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    shiftRightArithmetic(const InstructionSemantics2::BaseSemantics::SValuePtr &a,
                         const InstructionSemantics2::BaseSemantics::SValuePtr &nBits) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    unsignedExtend(const InstructionSemantics2::BaseSemantics::SValue::Ptr &a, size_t newWidth) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    signExtend(const InstructionSemantics2::BaseSemantics::SValue::Ptr &a, size_t newWidth) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    add(const InstructionSemantics2::BaseSemantics::SValuePtr &a,
        const InstructionSemantics2::BaseSemantics::SValuePtr &b) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    addCarry(const InstructionSemantics2::BaseSemantics::SValuePtr &a,
             const InstructionSemantics2::BaseSemantics::SValuePtr &b,
             InstructionSemantics2::BaseSemantics::SValuePtr &carryOut /*out*/,
             InstructionSemantics2::BaseSemantics::SValuePtr &overflowed /*out*/) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    subtract(const InstructionSemantics2::BaseSemantics::SValuePtr &a,
             const InstructionSemantics2::BaseSemantics::SValuePtr &b) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    subtractCarry(const InstructionSemantics2::BaseSemantics::SValuePtr &a,
                  const InstructionSemantics2::BaseSemantics::SValuePtr &b,
                  InstructionSemantics2::BaseSemantics::SValuePtr &carryOut /*out*/,
                  InstructionSemantics2::BaseSemantics::SValuePtr &overflowed /*out*/) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    addWithCarries(const InstructionSemantics2::BaseSemantics::SValuePtr &a,
                   const InstructionSemantics2::BaseSemantics::SValuePtr &b,
                   const InstructionSemantics2::BaseSemantics::SValuePtr &c,
                   InstructionSemantics2::BaseSemantics::SValuePtr &carryOut /*out*/) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor, const InstructionSemantics2::BaseSemantics::SValuePtr&) override;

    virtual void
    writeRegister(RegisterDescriptor, const InstructionSemantics2::BaseSemantics::SValuePtr&) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(RegisterDescriptor segreg, const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               const InstructionSemantics2::BaseSemantics::SValuePtr &cond) override;

    virtual void
    writeMemory(RegisterDescriptor segreg, const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                const InstructionSemantics2::BaseSemantics::SValuePtr &value,
                const InstructionSemantics2::BaseSemantics::SValuePtr &cond) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// High-level semantic operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Virtual definition of semantic operations for model checking. */
class SemanticCallbacks: public Rose::BinaryAnalysis::ModelChecker::SemanticCallbacks {
public:
    using Ptr = SemanticCallbacksPtr;
    using UnitCounts = Sawyer::Container::Map<rose_addr_t, size_t>;

private:
    Settings settings_;                                 // settings are set by the constructor and not modified thereafter
    const Partitioner2::Partitioner &partitioner_;      // generally shouldn't be changed once model checking starts
    SmtSolver::Memoizer::Ptr smtMemoizer_;              // memoizer shared among all solvers

    mutable SAWYER_THREAD_TRAITS::Mutex unitsMutex_;    // protects only the units_ data member
    Sawyer::Container::Map<rose_addr_t, ExecutionUnitPtr> units_; // cached execution units

    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    std::set<uint64_t> seenStates_;                     // states we've seen, by hash
    size_t nDuplicateStates_ = 0;                       // number of times a previous state is seen again
    size_t nSolverFailures_ = 0;                        // number of times the SMT solver failed
    UnitCounts unitsReached_;                           // number of times basic blocks were reached
    Variables::VariableFinderPtr variableFinder_;       // also shared by all RiscOperator objects

    // This model is able to follow a single path specified by the user. In order to do that, the user should call
    // followOnePath to provide the ordered list of execution units. The nextCodeAddresses function is overridden to
    // return only the address of the next execution unit in this list, or none when the list is empty. The findUnit
    // function is enhanced to return the next execution unit and remove it from the list.
    //
    // These are also protected by mutex_
    bool followingOnePath_ = false;
    std::list<ExecutionUnitPtr> onePath_;               // execution units yet to be consumed by findUnit

protected:
    SemanticCallbacks(const ModelChecker::SettingsPtr&, const Settings&, const Partitioner2::Partitioner&);
public:
    ~SemanticCallbacks();

public:
    static Ptr instance(const ModelChecker::SettingsPtr&, const Settings&, const Partitioner2::Partitioner&);

public:
    /** Property: Partitioner being used. */
    const Partitioner2::Partitioner& partitioner() const;

public:
    /** Cause this model to follow only one path through the specimen.
     *
     *  The path is provided as an ordered list of execution units. The initial execution unit will not appear in this list
     *  since it should have been added to the model checker work list already, and it must be the only work on that list. In
     *  this mode of operation, the model checker is single threaded and will follow only this specified path until it reaches
     *  the end or some other condition causes the model checker to abandon the path earlier.
     *
     *  When the model checker runs, the @ref nextCodeAddress method is overridden to return only the address of the next
     *  execution unit on this list, and the internal @ref findUnit function returns the next unit, removing it from the
     *  list. When the list becomes empty, then no more code addresses or units are returned.
     *
     *  If this function is called when the model checker is already in @ref followingOnePath mode, the behavior is as if
     *  @ref followingOnePath was cleared first.
     *
     *  Thread safety: This function is thread safe. */
    void followOnePath(const std::list<ExecutionUnitPtr>&);

    /** Property: Whether we are in follow-one-path mode.
     *
     *  This property can be queried or modified at any time. The @ref followOnePath function sets the property, which must
     *  then be cleared manually if you want to continue to use the model checker in its normal mode. Clearing this property
     *  will also discard any execution units that remain for the one path mode. Setting the property manually has no effect
     *  if the property was already set, but if it was clear then the model checker behaves as if it reached the end of the
     *  one path: it will stop exploring.
     *
     *  Thread safety: This property accessor is thread safe.
     *
     * @{ */
    bool followingOnePath() const;
    void followingOnePath(bool);
    /** @} */

    /** Property: SMT solver memoizer.
     *
     *  This is the memoizer used each time a new SMT solver is created. An initial memoizer is created by the @c
     *  SemanticCallbacks constructor if the @p solverMemoization field of the @ref Settings is set. If memoization is
     *  not enabled in the settings, then a memoizer is not used even if one is set for this property.
     *
     * @{ */
    SmtSolver::Memoizer::Ptr smtMemoizer() const;
    void smtMemoizer(const SmtSolver::Memoizer::Ptr&);
    /** @} */

    /** Property: Number of duplicate states.
     *
     *  This read-only property returns the number of times that a semantic state was encountered that had been encountered
     *  previously. Whenever a state is re-encountered, the corresponding path is not extended since extending it would also
     *  result in duplicate states (even though the paths to those states are different).
     *
     *  Thread safety: This property accessor is thread safe. */
    size_t nDuplicateStates() const;

    /** Property: Number of times the SMT solver failed.
     *
     *  This read-only property contains the number of times that an attempt was made to extend a path and the SMT solver
     *  returned an "unknown" answer (i.e., neither "satisfied" nor "unsatisfied"). An "unknown" result is usually caused
     *  only when a per-call solver timeout is enabled and the solver was unable to finish in the allotted time.
     *
     *  Thread safety: This property accessor is thread safe. */
    size_t nSolverFailures() const;

    /** Property: Number of execution units reached.
     *
     *  This read-only property returns the number of unique execution unit addresses that were reached (i.e., semantically
     *  executed) during exploration. An execution unit is normally either a basic block or an externally defined function.
     *  However, if we attempt to execute an instruction that wasn't known to the partitioner, then each such instruction
     *  is counted separately without being part of a basic block.
     *
     *  Thread safety: This property accessor is thread safe. */
    size_t nUnitsReached() const;

    /** Property: Execution units reached.
     *
     *  Returns a mapping from execution unit address to the number of times that the address was semantically executed. The
     *  counts are returned by value rather than reference in order to achieve thread safety.
     *
     *  Thread safety: This property accessor is thread safe. */
    UnitCounts unitsReached() const;

    /** Filter null dereferences.
     *
     *  Returns true to accept the nullptr dereference, or false to say that it's not really a nullptr dereference.
     *
     *  The default implementation always returns true.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual bool filterNullDeref(const InstructionSemantics2::BaseSemantics::SValuePtr &addr, SgAsmInstruction*,
                                 TestMode testMode, IoMode ioMode);

    /** Filter out of bounds access.
     *
     *  Returns true to accept an out of bounds access, or false to say that it's a false positive.
     *
     *  The arguments are the symbolic virtual starting address for the accessed memory (@p addr), the memory region that's
     *  associated with the symbolic address (@p referencedRegion), the memory region that the I/O operation is actually
     *  accessing (@p accessedRegion), the instruction that's accessing that memory region (@p insn), whether the accessed
     *  memory "must" or "may" be outside the referenced region (@p testMode), and whether the instruction is reading or
     *  writing to the memory (@p ioMode). The @p intendedVariable and @p intendedVariableLocation is information about the
     *  stack variable that was intended to be accessed and its location in memory. The @p accessedVariable and @p
     *  accessedVariableLocation describe a variable (perhaps one of many) that was actually accessed, if any.
     *
     *  The default implementation always returns true.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual bool filterOobAccess(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                                 const AddressInterval &referencedRegion, const AddressInterval &accessedRegion,
                                 SgAsmInstruction *insn, TestMode testMode, IoMode ioMode,
                                 const Variables::StackVariable &intendedVariable, const AddressInterval &intendedVariableLocation,
                                 const Variables::StackVariable &accessedVariable, const AddressInterval &accessedVariableLocation);

    virtual bool filterUninitVar(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                                 const AddressInterval &referencedREgion, const AddressInterval &accessedRegion,
                                 SgAsmInstruction *insn, TestMode testMode, const Variables::StackVariable &variable,
                                 const AddressInterval &variableLocation);

public:
    virtual void reset() override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr protoval() override;

    virtual InstructionSemantics2::BaseSemantics::RegisterStatePtr createInitialRegisters() override;

    virtual InstructionSemantics2::BaseSemantics::MemoryStatePtr createInitialMemory() override;

    virtual InstructionSemantics2::BaseSemantics::StatePtr createInitialState() override;

    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr createRiscOperators() override;

    virtual void
    initializeState(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual InstructionSemantics2::BaseSemantics::DispatcherPtr
    createDispatcher(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual SmtSolver::Ptr createSolver() override;

    virtual void attachModelCheckerSolver(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                                          const SmtSolver::Ptr&) override;

    virtual CodeAddresses
    nextCodeAddresses(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual std::vector<TagPtr>
    preExecute(const ExecutionUnitPtr&, const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual std::vector<TagPtr>
    postExecute(const ExecutionUnitPtr&, const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    instructionPointer(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual std::vector<NextUnit>
    nextUnits(const PathPtr&, const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&, const SmtSolver::Ptr&) override;

#ifdef ROSE_HAVE_LIBYAML
    virtual std::list<ExecutionUnitPtr>
    parsePath(const YAML::Node&, const std::string &sourceName) override;
#endif

private:
    // Records the state hash and returns true if we've seen it before.
    bool seenState(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&);

    // Find and cache the execution unit at the specified address.
    ExecutionUnitPtr findUnit(rose_addr_t va, const Progress::Ptr&);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
