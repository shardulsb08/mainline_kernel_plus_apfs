.. SPDX-License-Identifier: GPL-2.0

========================================
APFS (Apple File System) integration plan
========================================

Status
======

APFS support currently exists in this workspace as an out-of-tree module in
``linux-apfs-rw-master/``. The mainline tree in ``linux/`` does not yet include
functional APFS code under ``fs/``.

This document tracks the incremental in-tree integration strategy with a strict
"small steps, always buildable" approach.

Phase 0 expectations
====================

Phase 0 is a tree-integration and plumbing phase, *not* a feature-complete
filesystem phase.

If we stop at only Phase 0 scaffolding, APFS features should **not** be
expected to work yet (no mountable APFS implementation is provided at this
step).

The goal of Phase 0 is to establish proper in-tree structure and make future
patches reviewable by filesystem maintainers.

Kickoff sub-task (completed)
============================

The first minimal sub-task is to reserve APFS integration points in the kernel
filesystem build and config system:

- add ``fs/apfs/`` directory;
- add ``fs/apfs/Kconfig`` with a guarded placeholder option;
- add ``fs/apfs/Makefile`` scaffold;
- wire APFS Kconfig into ``fs/Kconfig``;
- wire APFS directory into ``fs/Makefile``.

This keeps the tree organized to upstream conventions while intentionally
avoiding premature feature claims.

Next tiny step
==============

Introduce a no-op APFS module stub under ``fs/apfs/`` (still marked non-user
ready), so CI can compile a concrete APFS object while we incrementally import
real functionality in follow-up patches.
