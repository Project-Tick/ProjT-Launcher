# SPDX-License-Identifier: GPL-3.0
# launcher/tasks - Task System Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/tasks

TASKS_SRCS := \
	ConcurrentTask.cpp \
	MultipleOptionsTask.cpp \
	SequentialTask.cpp \
	Task.cpp

TASKS_OBJS := $(addprefix $(obj)/, $(TASKS_SRCS:.cpp=.o))
TASKS_LIB := $(obj)/libtasks.a

# MOC
TASKS_MOC_HEADERS := $(shell find $(srctree)/launcher/tasks -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
TASKS_MOC_SRCS := $(patsubst $(srctree)/launcher/tasks/%.h,$(obj)/moc_%.cpp,$(TASKS_MOC_HEADERS))
TASKS_MOC_OBJS := $(TASKS_MOC_SRCS:.cpp=.o)

TASKS_CXXFLAGS := \
	-I$(srctree)/launcher/tasks \
	-I$(srctree)/launcher \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: tasks tasks-clean

tasks: $(TASKS_LIB)

$(TASKS_LIB): $(TASKS_OBJS) $(TASKS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/tasks/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(TASKS_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/tasks/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(TASKS_CXXFLAGS) -c -o $@ $<

tasks-clean:
	$(Q)rm -rf $(obj)

-include $(TASKS_OBJS:.o=.d)
