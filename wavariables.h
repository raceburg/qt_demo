#ifndef WAVARIABLES_H
#define WAVARIABLES_H
#include <QColor>

enum InstructionName
{
    setColorFraction,
    setColorAllFraction,
    showFraction,
    setOpacityObjects,
    showAllObjects,
    hideAllObjects,
    visibleObjects,
    unallocatedAreaShow,
    unallocatedAreaHide,
    manualLayer_SetOpacity,
    manualLayer_SetEditedAreaType,
    manualLayer_SetVisible,
    manualLayer_SetBrushSize,
    manualLayer_AddFullSizeImagePolygon,
    clearWorkLayers,
    ruler_setMetricLength,
    ruler_updatePixelLen,
    ruler_delete,
    showGrid,
    hideGrid
};

enum SceneAction {
    imageLoad,
    imageClose,
    updateColorFraction,
    updateBrushSize,
    setDefaultType,
    setRullerType,
    setSelectionType,
    setModifyObjectUsedBrush,
    setModifyObjectUsedPolygon,
    noAction
};

enum LoadImageType {
    // изображение для разметки
    baseImage,
    // маска размеченных объектов
    maskImage
};

enum ActionTypes {
    DefaultType,
    RullerType,
    SelectionType,
    // флаг, разметка в ручном режиме, используется для выделения отдельных областей
    ModifyObjectUsedBrush,
    ModifyObjectUsedPolygon
};


#endif // WAVARIABLES_H
