#pragma once

enum OpType {
    None,
    Load,
    Broadcast,
    BroadcastZero,
    ScalarStore,
    Store,
    HAdd,
    Add,
    Sub,
    Mul,
    Div,
    Extract
};

