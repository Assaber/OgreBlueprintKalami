#pragma once

#define GET_QT_METATYPE_ID(x) QMetaTypeId<x>::qt_metatype_id()

#define GROUPED_CELL_INNER_FUNC_BIND(function) std::bind(&function, this, std::placeholders::_1, std::placeholders::_2)