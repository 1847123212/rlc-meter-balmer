# Мысли о будущем. #

Перечислю недостатки.

**I/V конвертор на высоких частотах это плохо.** DAC таки вносит свою лепту в неточность. На низких частотах не хватает битности. На высоких частотах - недостаточна частота.
**Емкость конденсатора зависит от подаваемого на него напряжения и температуры. Вполне может приключиться такой случай, когда конденсатор на 1 nF в реальной схеме будет иметь емкость 200 pF.** У электролитических конденсаторов есть ток утечки зависящий от подаваемого напряжения, и его хотелось бы знать.
**Индуктивность катушки сильно зависит от протекающего тока.** На высоких частотах - щупы это зло. А индуктивность меньше 10 nH и емкость меньше 0.1 pF измерять отдельно безсмысленно. Они имеют смысл только в рамках общей схемы.


Перечисление недостатков дает нам ответ на вопрос, как улучшать параметры далее. Если хочется высоких частот - надо избавляться от I/V конвертора, а резисторы переключать при помощи реле (желательно высокочастотных).

DAC таки желательно использовать внешний. Минимум 16 бит, если нужны низкие частоты. DDS синтезатор, если нужны высокие частоты.

Если хочется точности лучше 0.5%, то таки желателен ADC 16 бит. Другой вариант как добавить пару бит точности - двуполярное питание ОУ. Это позволит использовать весь диапазон ADC для измерения.

Конденсаторы с большой точностью бесполезно измерять без указания температуры и прилагаемого напряжения. То есть таки термостабильная камера и схема прилагающая напряжение смещения. С катушкой индуктивности аналогично, только вместо напряжения смещения надо ток регулировать.

На высоких частотах желательно использовать кабели и стандартные высокочастотные разъемы.