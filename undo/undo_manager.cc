// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/undo/undo_manager.h"

#include "base/auto_reset.h"
#include "base/logging.h"
#include "chrome/browser/undo/undo_operation.h"

// UndoGroup ------------------------------------------------------------------

UndoGroup::UndoGroup() {
}

UndoGroup::~UndoGroup() {
}

void UndoGroup::AddOperation(scoped_ptr<UndoOperation> operation) {
  operations_.push_back(operation.release());
}

void UndoGroup::Undo() {
  for (ScopedVector<UndoOperation>::reverse_iterator ri = operations_.rbegin();
       ri != operations_.rend(); ++ri) {
    (*ri)->Undo();
  }
}

// UndoManager ----------------------------------------------------------------

UndoManager::UndoManager()
    : group_actions_count_(0),
      undo_suspended_count_(0),
      performing_undo_(false),
      performing_redo_(false) {
}

UndoManager::~UndoManager() {
  DCHECK_EQ(0, group_actions_count_);
  DCHECK_EQ(0, undo_suspended_count_);
  DCHECK(!performing_undo_);
  DCHECK(!performing_redo_);
}

void UndoManager::Undo() {
  Undo(&performing_undo_, &undo_actions_);
}

void UndoManager::Redo() {
  Undo(&performing_redo_, &redo_actions_);
}

void UndoManager::AddUndoOperation(scoped_ptr<UndoOperation> operation) {
  if (IsUndoTrakingSuspended()) {
    RemoveAllActions();
    operation.reset();
    return;
  }

  if (group_actions_count_) {
    pending_grouped_action_->AddOperation(operation.Pass());
  } else {
    UndoGroup* new_action = new UndoGroup();
    new_action->AddOperation(operation.Pass());
    GetActiveUndoGroup()->insert(GetActiveUndoGroup()->end(), new_action);

    // A new user action invalidates any available redo actions.
    RemoveAllRedoActions();
  }
}

void UndoManager::StartGroupingActions() {
  if (!group_actions_count_)
    pending_grouped_action_.reset(new UndoGroup());
  ++group_actions_count_;
}

void UndoManager::EndGroupingActions() {
  --group_actions_count_;
  if (group_actions_count_ > 0)
    return;

  // Check that StartGroupingActions and EndGroupingActions are paired.
  DCHECK_GE(group_actions_count_, 0);

  bool is_user_action = !performing_undo_ && !performing_redo_;
  if (pending_grouped_action_->has_operations()) {
    GetActiveUndoGroup()->push_back(pending_grouped_action_.release());
    // User actions invalidate any available redo actions.
    if (is_user_action)
      RemoveAllRedoActions();
  } else {
    // No changes were executed since we started grouping actions, so the
    // pending UndoGroup should be discarded.
    pending_grouped_action_.reset();

    // This situation is only expected when it is a user initiated action.
    // Undo/Redo should have at least one operation performed.
    DCHECK(is_user_action);
  }
}

void UndoManager::SuspendUndoTracking() {
  ++undo_suspended_count_;
}

void UndoManager::ResumeUndoTracking() {
  DCHECK_GT(undo_suspended_count_, 0);
  --undo_suspended_count_;
}

bool UndoManager::IsUndoTrakingSuspended() const {
  return undo_suspended_count_ > 0;
}

void UndoManager::Undo(bool* performing_indicator,
                       ScopedVector<UndoGroup>* active_undo_group) {
  // Check that action grouping has been correctly ended.
  DCHECK(!group_actions_count_);

  if (active_undo_group->empty())
    return;

  base::AutoReset<bool> incoming_changes(performing_indicator, true);
  scoped_ptr<UndoGroup> action(active_undo_group->back());
  active_undo_group->weak_erase(
      active_undo_group->begin() + active_undo_group->size() - 1);

  StartGroupingActions();
  action->Undo();
  EndGroupingActions();
}

void UndoManager::RemoveAllActions() {
  undo_actions_.clear();
  RemoveAllRedoActions();
}

void UndoManager::RemoveAllRedoActions() {
  redo_actions_.clear();
}

ScopedVector<UndoGroup>* UndoManager::GetActiveUndoGroup() {
  return performing_undo_ ? &redo_actions_ : &undo_actions_;
}
