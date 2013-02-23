/*!
 * Pikaday
 * Copyright © 2012 David Bushell | BSD & MIT license | http://dbushell.com/
 */

(function(window, document, undefined)
{
    'use strict';

    /**
     * feature detection and helper functions
     */
    var hasMoment = typeof window.moment === 'function',

    hasEventListeners = !!window.addEventListener,

    sto = window.setTimeout,

    addEvent = function(el, e, callback, capture)
    {
        if (hasEventListeners) {
            el.addEventListener(e, callback, !!capture);
        } else {
            el.attachEvent('on' + e, callback);
        }
    },

    removeEvent = function(el, e, callback, capture)
    {
        if (hasEventListeners) {
            el.removeEventListener(e, callback, !!capture);
        } else {
            el.detachEvent('on' + e, callback);
        }
    },

    trim = function(str)
    {
        return str.trim ? str.trim() : str.replace(/^\s+|\s+$/g,'');
    },

    hasClass = function(el, cn)
    {
        return (' ' + el.className + ' ').indexOf(' ' + cn + ' ') !== -1;
    },

    addClass = function(el, cn)
    {
        if (!hasClass(el, cn)) {
            el.className = (el.className === '') ? cn : el.className + ' ' + cn;
        }
    },

    removeClass = function(el, cn)
    {
        el.className = trim((' ' + el.className + ' ').replace(' ' + cn + ' ', ' '));
    },

    isArray = function(obj)
    {
        return (/Array/).test(Object.prototype.toString.call(obj));
    },

    isDate = function(obj)
    {
        return (/Date/).test(Object.prototype.toString.call(obj)) && !isNaN(obj.getTime());
    },

    isLeapYear = function(year)
    {
        // solution by Matti Virkkunen: http://stackoverflow.com/a/4881951
        return year % 4 === 0 && year % 100 !== 0 || year % 400 === 0;
    },

    getDaysInMonth = function(year, month)
    {
        return [31, isLeapYear(year) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31][month];
    },

    compareDates = function(a,b)
    {
        // weak date comparison (use date.setHours(0,0,0,0) to ensure correct result)
        return a.getTime() === b.getTime();
    },

    extend = function(to, from, overwrite)
    {
        var prop, hasProp;
        for (prop in from) {
            hasProp = to[prop] !== undefined;
            if (hasProp && typeof from[prop] === 'object' && from[prop].nodeName === undefined) {
                if (isDate(from[prop])) {
                    if (overwrite) {
                        to[prop] = new Date(from[prop].getTime());
                    }
                }
                else if (isArray(from[prop])) {
                    if (overwrite) {
                        to[prop] = from[prop].slice(0);
                    }
                } else {
                    to[prop] = extend({}, from[prop], overwrite);
                }
            } else if (overwrite || !hasProp) {
                to[prop] = from[prop];
            }
        }
        return to;
    },


    /**
     * defaults and localisation
     */
    defaults = {

        // bind the picker to a form field
        field: null,
        
        // create/bind to a button calendar button
        calbutton: null,

        // automatically show/hide the picker on `field` focus (default `true` if `field` is set)
        bound: true,
        
        // the default output format for `.toString()` and `field` value
        format: 'YYYY-MM-DD',

        // the initial date to view when first opened
        defaultDate: null,

        // make the `defaultDate` the initial selected value
        setDefaultDate: false,

        // first day of week (0: Sunday, 1: Monday etc)
        firstDay: 0,
        
        // choose if time is to be used
        useTime: false,
        
        // whether or not to use seconds
        useSecs: false,
        
        // if information should be confirmed
        confirm: false,
        
        // which format preset to use
        formatPreset: 0,

        // the minimum/earliest date that can be selected
        minDate: null,
        // the maximum/latest date that can be selected
        maxDate: null,

        // number of years either side, or array of upper/lower range
        yearRange: 10,

        // used internally (don't config outside)
        minYear: 0,
        maxYear: 9999,
        minMonth: undefined,
        maxMonth: undefined,

        isRTL: false,

        // how many months are visible (not implemented yet)
        numberOfMonths: 1,

        // internationalization
        i18n: {

                months        : ['January','February','March','April','May','June','July','August','September','October','November','December'],
                weekdays      : ['Sunday','Monday','Tuesday','Wednesday','Thursday','Friday','Saturday'],
                weekdaysShort : ['Sun','Mon','Tue','Wed','Thu','Fri','Sat']
        },

        // callback function
        onSelect: null,
        onOpen: null,
        onClose: null
    },


    /**
     * templating functions to abstract HTML rendering
     */
    renderDayName = function(opts, day, abbr)
    {
        day += opts.firstDay;
        while (day >= 7) {
            day -= 7;
        }
        return abbr ? opts.i18n.weekdaysShort[day] : opts.i18n.weekdays[day];
    },

    renderDay = function(i, isSelected, isToday, isDisabled, isEmpty)
    {
        if (isEmpty) {
            return '<td class="is-empty"></td>';
        }
        var arr = [];
        if (isDisabled) {
            arr.push('is-disabled');
        }
        if (isToday) {
            arr.push('is-today');
        }
        if (isSelected) {
            arr.push('is-selected');
        }
        return '<td data-day="' + i + '" class="' + arr.join(' ') + '"><button class="pika-button" type="button">' + i + '</button>' + '</td>';
    },

    renderRow = function(days, isRTL)
    {
        return '<tr>' + (isRTL ? days.reverse() : days).join('') + '</tr>';
    },

    renderBody = function(rows)
    {
        return '<tbody>' + rows.join('') + '</tbody>';
    },

    renderHead = function(opts)
    {
        var i, arr = [];
        for (i = 0; i < 7; i++) {
            arr.push('<th scope="col"><abbr title="' + renderDayName(opts, i) + '">' + renderDayName(opts, i, true) + '</abbr></th>');
        }
        return '<thead>' + (opts.isRTL ? arr.reverse() : arr).join('') + '</thead>';
    },

    renderTitle = function(instance)
    {
        var i, j, arr,
            opts = instance._o,
            month = instance._m,
            year  = instance._y,
            isMinYear = year === opts.minYear,
            isMaxYear = year === opts.maxYear,
            html = '<div class="pika-title">',
            yearPrev = true,
            yearNext = true,
            prev = true,
            next = true;

        for (arr = [], i = 0; i < 12; i++) {
            arr.push('<option value="' + i + '"' +
                (i === month ? ' selected': '') +
                ((isMinYear && i < opts.minMonth) || (isMaxYear && i > opts.maxMonth) ? 'disabled' : '') + '>' +
                opts.i18n.months[i] + '</option>');
        }
        html += '<div class="pika-label">' + opts.i18n.months[month] + '<select class="pika-select pika-select-month">' + arr.join('') + '</select></div>';

        if (isArray(opts.yearRange)) {
            i = opts.yearRange[0];
            j = opts.yearRange[1] + 1;
        } else {
            i = year - opts.yearRange;
            j = 1 + year + opts.yearRange;
        }

        for (arr = []; i < j && i <= opts.maxYear; i++) {
            if (i >= opts.minYear) {
                arr.push('<option value="' + i + '"' + (i === year ? ' selected': '') + '>' + (i) + '</option>');
            }
        }
        html += '<div class="pika-label">' + year + '<select class="pika-select pika-select-year">' + arr.join('') + '</select></div>';

        if (isMinYear) {
            yearPrev = false;
            if (month === 0 || opts.minMonth >= month) {
                prev = false;
            }
        }

        if (isMaxYear) {
            yearNext = false;
            if (month === 11 || opts.maxMonth <= month) {
                next = false;
            }
        }

        html += '<button class="pika-year-prev' + (yearPrev ? '' : ' is-disabled') + '" type="button">Previous Year</button>';
        html += '<button class="pika-year-next' + (yearNext ? '' : ' is-disabled') + '" type="button">Next Year</button>';
        html += '<button class="pika-prev' + (prev ? '' : ' is-disabled') + '" type="button">Previous Month</button>';
        html += '<button class="pika-next' + (next ? '' : ' is-disabled') + '" type="button">Next Month</button>';

        return html += '</div>';
    },

    renderTable = function(opts, data)
    {
        return '<table cellpadding="0" cellspacing="0" class="pika-table">' + renderHead(opts) + renderBody(data) + '</table>';
    },
    
    renderTime = function(instance)
    {
        var i, j, arr,
            opts = instance._o,
            second = instance._s,
            minute = instance._mi,
            hour   = instance._h,
            html = '<div class="pika-time">',
            prevHour = true,
            nextHour = true,
            prevMinute = true,
            nextMinute = true,
            prevSecond = true,
            nextSecond = true;
            
        if (hour === 0) {
            prevHour = false;
            if (minute === 0) {
                prevMinute = false;
                if (second === 0) {
                    prevMinute = false;
                }
            }
        }

        if (hour === 23) {
            nextHour = false;
            if (minute === 59) {
                nextMinute = false;
                if (second === 59) {
                    prevSecond = false;
                }
            }
        }

        for (arr = [], i = 0; i < 24; i+= 2) {
            if (i) {
                arr.push('<option value="' + (i-2) + '"' + ((i-2) === hour ? ' selected': '') + '>' + ((i-2) >= 10 ? (i-2) : '0' + (i-2)) + '</option>');
            }
            else {
                arr.push('<option value="default" ' + ( (hour % 2 || hour == 'undefined') ? 'selected ': '') + 'disabled>Select Hour:</option>');
                self._w = false;
            }
        }
        html += '<button class="pika-hour-prev' + (prevHour ? '' : ' is-disabled') + '" type="button">Previous Hour</button>';
        html += '<div class="pika-label-time">' + (hour >= 10 ? hour : '0' + hour) + '<select class="pika-select pika-select-hour">' + arr.join('') + '</select></div>';
        html += '<button class="pika-hour-next' + (nextHour ? '' : ' is-disabled') + '" type="button">Next Hour</button>';

        for (arr = [], i = 0; i < 60; i+=5) {
            if (i) {
                arr.push('<option value="' + (i-5) + '"' + ((i-5) === minute ? ' selected': '') + '>' + ((i-5) >= 10 ? (i-5) : '0' + (i-5)) + '</option>');
            }
            else {
                arr.push('<option value="default" ' + ( (minute % 5 || minute == 'undefined') ? 'selected ': '') + 'disabled>Select Minutes:</option>');
            }
        }
        html += '<div class="pika-label-time">:</div>';
        html += '<button class="pika-minute-prev' + (prevMinute ? '' : ' is-disabled') + '" type="button">Previous Minute</button>';
        html += '<div class="pika-label-time">' + (minute >= 10 ? minute : '0' + minute) + '<select class="pika-select pika-select-minute">' + arr.join('') + '</select></div>';
        html += '<button class="pika-minute-next' + (nextMinute ? '' : ' is-disabled') + '" type="button">Next Minute</button>';
        
        if (opts.useSecs) {
            for (arr = [], i = 0; i < 60; i+=5) {
                if (i) {
                    arr.push('<option value="' + (i-5) + '"' + ((i-5) === second ? ' selected': '') + '>' + ((i-5) >= 10 ? (i-5) : '0' + (i-5)) + '</option>');
                }
                else {
                    arr.push('<option value="default" ' + ( (second % 5 || second == 'undefined') ? 'selected ': '') + 'disabled>Select Seconds:</option>');
                }
            }
            html += '<div class="pika-label-time">:</div>';
            html += '<button class="pika-second-prev' + (prevSecond ? '' : ' is-disabled') + '" type="button">Previous Second</button>';
            html += '<div class="pika-label-time">' + (second >= 10 ? second : '0' + second) + '<select class="pika-select pika-select-second">' + arr.join('') + '</select></div>';
            html += '<button class="pika-second-next' + (nextSecond ? '' : ' is-disabled') + '" type="button">Next second</button>';
        }
        
        return html += '</div>';
    },
    
    renderConfirm = function() {
        return '<div class="pika-confirm""><button class="pika-confirm-button" type="button">OK</button></div>';
    };


    /**
     * Pikaday constructor
     */
    window.Pikaday = function(options)
    {
        var self = this,
            opts = self.config(options);

        self._onMouseDown = function(e)
        {
            if (!self._v) {
                return;
            }
            e = e || window.event;
            var target = e.target || e.srcElement;
            if (!target) {
                return;
            }

            if (!hasClass(target, 'is-disabled')) {
                if (hasClass(target, 'pika-button') && !hasClass(target, 'is-empty')) {
                    self._da = parseInt(target.innerHTML, 10);
                    self.draw();
                    
                    if (opts.confirm){
                        self._c = true;
                    } else {
                        self.setDate(new Date(self._y, self._m, self._da, self._h, self._mi, self._s));
                        if (opts.bound && !opts.useTime) {
                            sto(function() {
                                self.hide();
                            }, 100);
                        }
                        return;
                    }
                }
                else if (hasClass(target, 'pika-year-prev')) {
                    self.prevYear();
                }
                else if (hasClass(target, 'pika-year-next')) {
                    self.nextYear();
                }
                else if (hasClass(target, 'pika-prev')) {
                    self.prevMonth();
                }
                else if (hasClass(target, 'pika-next')) {
                    self.nextMonth();
                }
                else if (hasClass(target, 'pika-hour-prev')) {
                    self.prevHour();
                }
                else if (hasClass(target, 'pika-hour-next')) {
                    self.nextHour();
                }
                else if (hasClass(target, 'pika-minute-prev')) {
                    self.prevMinute();
                }
                else if (hasClass(target, 'pika-minute-next')) {
                    self.nextMinute();
                }
                else if (hasClass(target, 'pika-second-prev')) {
                    self.prevSecond();
                }
                else if (hasClass(target, 'pika-second-next')) {
                    self.nextSecond();
                }
                else if (hasClass(target, 'pika-confirm-button')) {
                    self.setDate(new Date(self._y, self._m, self._da, self._h, self._mi, self._s));
                    if (opts.bound) {
                        sto(function() {
                            self.hide();
                        }, 100);
                    }
                }
                if (!opts.confirm && !hasClass(target, 'pika-select')) {
                    self.setDate(new Date(self._y, self._m, self._da, self._h, self._mi, self._s));
                } 
            }
            if (!hasClass(target, 'pika-select')) {
                if (e.preventDefault) {
                    e.preventDefault();
                } else {
                    return e.returnValue = false;
                }
            } else {
                self._c = true;     /**************************************************/
            }
        };

        self._onChange = function(e)
        {
            e = e || window.event;
            var target = e.target || e.srcElement;
            if (!target) {
                return;
            }
            if (hasClass(target, 'pika-select-year')) {
                self.gotoYear(target.value);
            }
            else if (hasClass(target, 'pika-select-month')) {
                self.gotoMonth(target.value);
            }
            else if (hasClass(target,'pika-select-hour')) {
                self.gotoHour(target.value);
            }
            else if (hasClass(target,'pika-select-minute')) {
                self.gotoMinute(target.value);
            }
            else if (hasClass(target,'pika-select-second')) {
                self.gotoSecond(target.value);
            }
        };

        self._onInputChange = function(e)
        {
            if (!self._v) {
                var date;
                if (self._o.formatPreset == 1) {
                    date = new Date(Date.parse(opts.field.value.substr(0, 10)));
                    date.setHours(opts.field.value.substr(11, 2));
                    date.setMinutes(opts.field.value.substr(14, 2));
                    date.setSeconds(opts.field.value.substr(17, 2));
                }
                else {
                    date = new Date(Date.parse(opts.field.value));
                }
                self.setDate(isDate(date) ? date : null);
                self.show();
            }
        };

        self._onInputFocus = function(e)
        {
            self.show();
        };

        self._onInputClick = function(e)
        {
            self.show();
        };

        self._onInputBlur = function(e)     /**************************************************/
        {
            if (!self._c && !self._o.calbutton) {
                self._b = sto(function() {
                    self.hide();
                }, 50);
            }
            self._c = false;
        };

        self._onClick = function(e)
        {
            e = e || window.event;
            var target = e.target || e.srcElement,
                pEl = target;
            if (!target) {
                return;
            }
            if (hasClass(target, 'pika-select')) {
                if (!hasEventListeners && !target.onchange) {
                    target.setAttribute('onchange', 'return;');
                    addEvent(target, 'change', self._onChange);
                }
            }
            do {
                if (hasClass(pEl, 'pika-single')) {
                    return;
                }
            }
            while ((pEl = pEl.parentNode));
            if (self._v && target !== (opts.calbutton ? opts.calbutton : opts.field) ) {
                self._o.confirm ? '' : self.setDate(new Date(self._y, self._m, self._da, self._h, self._mi, self._s));;
                self.hide();
            }
        };

        self.el = document.createElement('div');
        self.el.className = 'pika-single' + (opts.isRTL ? ' is-rtl' : '');
        addEvent(self.el, 'mousedown', self._onMouseDown, true);
        addEvent(self.el, 'change', self._onChange);

        if (opts.field) {
            if (opts.bound) {
                document.body.appendChild(self.el);
            } else {
                opts.field.parentNode.insertBefore(self.el, opts.field.nextSibling);
            }
            addEvent(opts.field, 'change', self._onInputChange);
            
            if (!opts.defaultDate) {
                if (hasMoment && opts.field.value) {
                    opts.defaultDate = window.moment(opts.field.value, opts.format).toDate();
                } else {
                    opts.defaultDate = new Date(Date.parse(opts.field.value));
                }
                opts.setDefaultDate = true;
            }
        }

        var defDate = opts.defaultDate;

        if (isDate(defDate)) {
            if (opts.setDefaultDate) {
                self.setDate(defDate);
            } else {
                self.gotoDate(defDate);
            }
        } else {
            self.gotoDate(new Date());
        }

        if (opts.bound) {
            this.hide();
            self.el.className += ' is-bound';
            
            if(opts.calbutton) {
                addEvent(opts.calbutton, 'click', self._onInputClick);
            } else {
                addEvent(opts.field, 'click', self._onInputClick);
                addEvent(opts.field, 'focus', self._onInputFocus);
            }
            addEvent(opts.field, 'blur', self._onInputBlur);
        } else {
            this.show();
        }

    };


    /**
     * public Pikaday API
     */
    window.Pikaday.prototype = {


        /**
         * configure functionality
         */
        config: function(options)
        {
            if (!this._o) {
                this._o = extend({}, defaults, true);
            }

            var opts = extend(this._o, options, true);

            opts.isRTL = !!opts.isRTL;

            opts.field = (opts.field && opts.field.nodeName) ? opts.field : null;
            
            opts.calbutton = (opts.calbutton && opts.calbutton.nodeName) ? opts.calbutton : null;

            opts.bound = !!(opts.bound !== undefined ? opts.field && opts.bound : opts.field);

            var nom = parseInt(opts.numberOfMonths, 10) || 1;
            opts.numberOfMonths = nom > 4 ? 4 : nom;

            if (!isDate(opts.minDate)) {
                opts.minDate = false;
            }
            if (!isDate(opts.maxDate)) {
                opts.maxDate = false;
            }
            if ((opts.minDate && opts.maxDate) && opts.maxDate < opts.minDate) {
                opts.maxDate = opts.minDate = false;
            }
            if (opts.minDate) {
                opts.minYear = opts.minDate.getFullYear();
                opts.minMonth = opts.minDate.getMonth();
            }
            if (opts.maxDate) {
                opts.maxYear = opts.maxDate.getFullYear();
                opts.maxMonth = opts.maxDate.getMonth();
            }

            if (isArray(opts.yearRange)) {
                var fallback = new Date().getFullYear() - 10;
                opts.yearRange[0] = parseInt(opts.yearRange[0], 10) || fallback;
                opts.yearRange[1] = parseInt(opts.yearRange[1], 10) || fallback;
            } else {
                opts.yearRange = Math.abs(parseInt(opts.yearRange, 10)) || defaults.yearRange;
                if (opts.yearRange > 100) {
                    opts.yearRange = 100;
                }
            }

            return opts;
        },

        /**
         * return a formatted string of the current selection (using Moment.js if available)
         */
        toString: function(format)
        {
            var string = null,
                unformattedString = null;
            if (isDate(this._d)) {
                if (hasMoment) {
                    string = window.moment(this._d).format(format || this._o.format);
                } else {
                    if (this._o.formatPreset == 0) {
                        string = this._d.toDateString();
                        if (this._o.useTime) {
                            string += (' ' + this._d.toTimeString().substr(0,5));
                            if (this._o.useSecs) {
                                string += this._d.toTimeString().substr(5,3);
                            }
                        }
                    }
                    else if (this._o.formatPreset == 1) {
                        if (this._d.toISOString) {
                            string = this._d.toISOString().substr(0,10)
                	        if (this._o.useTime) {
                	            string += (' ' + this._d.toTimeString().substr(0,5));
                                if (this._o.useSecs) {
                                    string += this._d.toTimeString().substr(5,3);
                                }
                            }
                        } else {
                            month = this._d.getMonth() + 1;
                            day = this._d.getDate();
                            string = this._d.getFullYear() + '-';
                            string += (month < 10 ? '0' + month : month)  + '-';
                            string += (day < 10 ? '0' + day : day);
                	        if (this._o.useTime) {
                	            string += (' ' + this._d.toTimeString().substr(0,5));
                                if (this._o.useSecs) {
                                    string += this._d.toTimeString().substr(5,3);
                                }
                            }
                        }
                    } else {
                        var message = 'Error: formatPreset: Not a preset'
                        if (typeof console == 'undefined') {
                            alert(message);
                        } else {
                            console.error(message); 
                        }
                    }
                }
            }
            return string;
        },

        /**
         * return a Moment.js object of the current selection (if available)
         */
        getMoment: function()
        {
            return hasMoment ? window.moment(this._d) : null;
        },

        /**
         * return a Date object of the current selection
         */
        getDate: function()
        {
            return isDate(this._d) ? new Date(this._d.getTime()) : null;
        },

        /**
         * set the current selection
         */
        setDate: function(date)
        {
            if (!date) {
                this._d = null;
                return this.draw();
            }
            if (typeof date === 'string') {
                date = new Date(Date.parse(date.toString()));
            }
            if (!isDate(date)) {
                return;
            }

            var min = this._o.minDate,
                max = this._o.maxDate;

            if (isDate(min) && date < min) {
                date = min;
            } else if (isDate(max) && date > max) {
                date = max;
            }

            this._d = new Date(date.getTime());
            this.gotoDate(this._d);
            if (this._o.field) {
                this._o.field.value = this.toString();
            }
            if (typeof this._o.onSelect === 'function') {
                this._o.onSelect.call(this, this.getDate());
            }
        },

        /**
         * change view to a specific date
         */
        gotoDate: function(date)
        {
            if (!isDate(date)) {
                return;
            }
            this._y = date.getFullYear();
            this._m = date.getMonth();
            this._da = date.getDate();
            this._h = date.getHours();
            this._mi = date.getMinutes();
            this._s = date.getSeconds();
            this.draw();
        },

        gotoToday: function()
        {
            this.gotoDate(new Date());
        },

        nextYear: function()
        {
            this._y++;
            this.draw();
        },

        prevYear: function()
        {
            this._y--;
            this.draw();
        },
        
        nextMonth: function()
        {
            if (++this._m > 11) {
                this._m = 0;
                this._y++;
            }
            this.draw();
        },

        prevMonth: function()
        {
            if (--this._m < 0) {
                this._m = 11;
                this._y--;
            }
            this.draw();
        },
        
        nextHour: function()
        {
            if (++this._h > 23) {
                this._h = 0;
                this._m++;
            }
            this.draw();
        },
        
        prevHour: function()
        {
            if (--this._h < 0) {
                this._h = 23;
                this._m--;
            }
            this.draw();
        },
        
        nextMinute: function()
        {
            if (++this._mi > 59) {
                this._mi = 0;
                this._h++;
            }
            this.draw();
        },
        
        prevMinute: function()
        {
            if (--this._mi < 0) {
                this._mi = 59;
                this._h--;
            }
            this.draw();
        },
        
        nextSecond: function()
        {
            if (++this._s > 59) {
                this._s = 0;
                this._mi++;
            }
            this.draw();
        },

        prevSecond: function()
        {
            if (--this._s < 0) {
                this._s = 59;
                this._mi--;
            }
            this.draw();
        },
        
        /**
         * change view to a specific full year (e.g. "2012")
         */
        gotoYear: function(year)
        {
            if (!isNaN(year)) {
                this._y = parseInt(year, 10);
                this.draw();
            }
        },
        
        /**
         * change view to a specific month (zero-index, e.g. 0: January)
         */
        gotoMonth: function(month)
        {
            if (!isNaN( (month = parseInt(month, 10)) )) {
                this._m = month < 0 ? 0 : month > 11 ? 11 : month;
                this.draw();
            }
        },
        
        gotoHour: function(hour)
        {
            if (!isNaN(hour)) {
                this._h = parseInt(hour, 10);
                this.draw();
            }
        },
        
        gotoMinute: function(minute)
        {
            if (!isNaN(minute)) {
                this._mi = parseInt(minute, 10);
                this.draw();
            }
        },
        
        gotoSecond: function(second)
        {
            if (!isNaN(second)) {
                this._s = parseInt(second, 10);
                this.draw();
            }
        },

        /**
         * refresh the HTML
         */
        draw: function(force)
        {
            if (!this._v && !force) {
                return;
            }
            var opts = this._o,
                minYear = opts.minYear,
                maxYear = opts.maxYear,
                minMonth = opts.minMonth,
                maxMonth = opts.maxMonth;

            if (this._y <= minYear) {
                this._y = minYear;
                if (!isNaN(minMonth) && this._m < minMonth) {
                    this._m = minMonth;
                }
            }
            if (this._y >= maxYear) {
                this._y = maxYear;
                if (!isNaN(maxMonth) && this._m > maxMonth) {
                    this._m = maxMonth;
                }
            }
            
            this.el.innerHTML = renderTitle(this) + this.render(this._y, this._m, this._da) + (opts.useTime ? renderTime(this) : '') + (opts.confirm ? renderConfirm(): '');

            if (opts.bound) {
                var pEl  = opts.field,
                    left = pEl.offsetLeft,
                    top  = pEl.offsetTop + pEl.offsetHeight;
                while((pEl = pEl.offsetParent)) {
                    left += pEl.offsetLeft;
                    top  += pEl.offsetTop;
                }
                
                this.el.style.cssText = 'position:absolute;left:' + left + 'px;top:' + top + 'px;';
                /*sto(function() {
                    opts.field.focus();
                }, 1);*/
            }
        },

        /**
         * render HTML for a particular month
         */
        render: function(year, month, day) /****************************************************************************/
        {
            var opts   = this._o,
                now    = new Date(),
                days   = getDaysInMonth(year, month),
                before = new Date(year, month, 1).getDay(),
                data   = [],
                row    = [];
            now.setHours(0,0,0,0);
            if (opts.firstDay > 0) {
                before -= opts.firstDay;
                if (before < 0) {
                    before += 7;
                }
            }
            var cells = days + before,
                after = cells;
            while(after > 7) {
                after -= 7;
            }
            cells += 7 - after;
            for (var i = 0, r = 0; i < cells; i++)
            {
                var dayDate = new Date(year, month, 1 + (i - before)),
                    isDisabled = (opts.minDate && dayDate < opts.minDate) || (opts.maxDate && dayDate > opts.maxDate),
                    isSelected = (i === day - 1 + before) ? true : false,
                    isToday = compareDates(dayDate, now),
                    isEmpty = i < before || i >= (days + before);

                row.push(renderDay(1 + (i - before), isSelected, isToday, isDisabled, isEmpty));

                if (++r === 7) {
                    data.push(renderRow(row, opts.isRTL));
                    row = [];
                    r = 0;
                }
            }
            return renderTable(opts, data);
        },

        isVisible: function()
        {
            return this._v;
        },

        show: function()
        {
            if (!this._v) {
                if (hasMoment) {
                    self.setDate(window.moment(opts.field.value, opts.format).toDate());
                }
                else {
                    var date;
                    if(this._o.field.value) {
                        if (this._o.formatPreset == 1) {
                            this._y = parseInt(this._o.field.value.substr(0, 4), 10);
                            this._m = parseInt(this._o.field.value.substr(5, 2), 10) - 1;
                            this._da = parseInt(this._o.field.value.substr(8, 2), 10);
                            if (this._o.useTime) {
                                this._h = parseInt(this._o.field.value.substr(11, 2), 10);
                                this._mi = parseInt(this._o.field.value.substr(14, 2), 10);
                                if (this._o.useSecs) {
                                    this._s = parseInt(this._o.field.value.substr(17, 2), 10);
                                }
                            }
                            date = new Date(this._y, this._m, this._da, this._h, this._mi, this._s);
                        }
                        else {
                            date = new Date(Date.parse(this._o.field.value));
                        }
                        this.setDate(isDate(date) ? date : null);
                    } else {
                        date = new Date;
                    }
                }
                if (this._o.bound) {
                    addEvent(document, 'click', this._onClick);
                    //addEvent(document, )
                }
                removeClass(this.el, 'is-hidden');
                this._v = true;
                this.draw();
                if (typeof this._o.onOpen === 'function') {
                    this._o.onOpen.call(this);
                }
            }
        },

        hide: function()
        {
            var v = this._v;
            if (v !== false) {
                if (this._o.bound) {
                    removeEvent(document, 'click', this._onClick);
                }
                this.el.style.cssText = '';
                addClass(this.el, 'is-hidden');
                this._v = false;
                if (v !== undefined && typeof this._o.onClose === 'function') {
                    this._o.onClose.call(this);
                }
            }
        },

        /**
         * GAME OVER
         */
        destroy: function()
        {
            this.hide();
            removeEvent(this.el, 'mousedown', this._onMouseDown, true);
            removeEvent(this.el, 'change', this._onChange);
            if (this._o.field) {
                removeEvent(this._o.field, 'change', this._onInputChange);
                if (this._o.bound) {
                    if(opts.calbutton) {
                        removeEvent(this._o.calbutton, 'click', this._onInputClick);
                    } else {
                        removeEvent(this._o.field, 'click', this._onInputClick);
                        removeEvent(this._o.field, 'focus', this._onInputFocus);
                    }
                    removeEvent(this._o.field, 'blur', this._onInputBlur);
                }
            }
            if (this.el.parentNode) {
                this.el.parentNode.removeChild(this.el);
            }
        }

    };

})(window, window.document);
