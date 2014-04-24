/*http://pan.baidu.com/res/static/js/import/yun_home_speed_all.js?t=201404241528*/
(function (N0, C1) {
	var y0 = N0.document,
	M0 = N0.navigator,
	P = N0.location,
	n = (function () {
		var J = function (_, A) {
			return new J.fn.init(_, A, B);
		},
		C = N0.jQuery,
		I = N0.$,
		B,
		P = /^(?:[^#<]*(<[\w\W]+>)[^>]*$|#([\w\-]*)$)/,
		e = /\S/,
		Q = /^\s+/,
		S = /\s+$/,
		H = /^<(\w+)\s*\/?>(?:<\/\1>)?$/,
		K = /^[\],:{}\s]*$/,
		d = /\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g,
		T = /"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g,
		a = /(?:^|:|,)(?:\s*\[)+/g,
		c = /(webkit)[ \/]([\w.]+)/,
		G = /(opera)(?:.*version)?[ \/]([\w.]+)/,
		O = /(msie) ([\w.]+)/,
		b = /(mozilla)(?:.*? rv:([\w.]+))?/,
		M = /-([a-z]|[0-9])/ig,
		F = /^-ms-/,
		V = function (A, _) {
			return (_ + "").toUpperCase();
		},
		U = M0.userAgent,
		_,
		W,
		Y,
		E = Object.prototype.toString,
		N = Object.prototype.hasOwnProperty,
		X = Array.prototype.push,
		R = Array.prototype.slice,
		D = String.prototype.trim,
		A = Array.prototype.indexOf,
		Z = {};
		J.fn = J.prototype = {
			constructor : J,
			init : function (D, E, C) {
				var _,
				B,
				F,
				A;
				if (!D) {
					return this;
				}
				if (D.nodeType) {
					this.context = this[0] = D;
					this.length = 1;
					return this;
				}
				if (D === "body" && !E && y0.body) {
					this.context = y0;
					this[0] = y0.body;
					this.selector = D;
					this.length = 1;
					return this;
				}
				if (typeof D === "string") {
					if (D.charAt(0) === "<" && D.charAt(D.length - 1) === ">" && D.length >= 3) {
						_ = [null, D, null];
					} else {
						_ = P.exec(D);
					}
					if (_ && (_[1] || !E)) {
						if (_[1]) {
							E = E instanceof J ? E[0] : E;
							A = (E ? E.ownerDocument || E : y0);
							F = H.exec(D);
							if (F) {
								if (J.isPlainObject(E)) {
									D = [y0.createElement(F[1])];
									J.fn.attr.call(D, E, true);
								} else {
									D = [A.createElement(F[1])];
								}
							} else {
								F = J.buildFragment([_[1]], [A]);
								D = (F.cacheable ? J.clone(F.fragment) : F.fragment).childNodes;
							}
							return J.merge(this, D);
						} else {
							B = y0.getElementById(_[2]);
							if (B && B.parentNode) {
								if (B.id !== _[2]) {
									return C.find(D);
								}
								this.length = 1;
								this[0] = B;
							}
							this.context = y0;
							this.selector = D;
							return this;
						}
					} else {
						if (!E || E.jquery) {
							return (E || C).find(D);
						} else {
							return this.constructor(E).find(D);
						}
					}
				} else {
					if (J.isFunction(D)) {
						return C.ready(D);
					}
				}
				if (D.selector !== C1) {
					this.selector = D.selector;
					this.context = D.context;
				}
				return J.makeArray(D, this);
			},
			selector : "",
			jquery : "1.7.2",
			length : 0,
			size : function () {
				return this.length;
			},
			toArray : function () {
				return R.call(this, 0);
			},
			get : function (_) {
				return _ == null ? this.toArray() : (_ < 0 ? this[this.length + _] : this[_]);
			},
			pushStack : function (A, C, _) {
				var B = this.constructor();
				if (J.isArray(A)) {
					X.apply(B, A);
				} else {
					J.merge(B, A);
				}
				B.prevObject = this;
				B.context = this.context;
				if (C === "find") {
					B.selector = this.selector + (this.selector ? " " : "") + _;
				} else {
					if (C) {
						B.selector = this.selector + "." + C + "(" + _ + ")";
					}
				}
				return B;
			},
			each : function (_, A) {
				return J.each(this, _, A);
			},
			ready : function (_) {
				J.bindReady();
				W.add(_);
				return this;
			},
			eq : function (_) {
				_ = +_;
				return _ === -1 ? this.slice(_) : this.slice(_, _ + 1);
			},
			first : function () {
				return this.eq(0);
			},
			last : function () {
				return this.eq(-1);
			},
			slice : function () {
				return this.pushStack(R.apply(this, arguments), "slice", R.call(arguments).join(","));
			},
			map : function (_) {
				return this.pushStack(J.map(this, function (B, A) {
						return _.call(B, A, B);
					}));
			},
			end : function () {
				return this.prevObject || this.constructor(null);
			},
			push : X,
			sort : [].sort,
			splice : [].splice
		};
		J.fn.init.prototype = J.fn;
		J.extend = J.fn.extend = function () {
			var I,
			H,
			D,
			C,
			G,
			E,
			B = arguments[0] || {},
			_ = 1,
			A = arguments.length,
			F = false;
			if (typeof B === "boolean") {
				F = B;
				B = arguments[1] || {};
				_ = 2;
			}
			if (typeof B !== "object" && !J.isFunction(B)) {
				B = {};
			}
			if (A === _) {
				B = this;
				--_;
			}
			for (; _ < A; _++) {
				if ((I = arguments[_]) != null) {
					for (H in I) {
						D = B[H];
						C = I[H];
						if (B === C) {
							continue;
						}
						if (F && C && (J.isPlainObject(C) || (G = J.isArray(C)))) {
							if (G) {
								G = false;
								E = D && J.isArray(D) ? D : [];
							} else {
								E = D && J.isPlainObject(D) ? D : {};
							}
							B[H] = J.extend(F, E, C);
						} else {
							if (C !== C1) {
								B[H] = C;
							}
						}
					}
				}
			}
			return B;
		};
		J.extend({
			noConflict : function (_) {
				if (N0.$ === J) {
					N0.$ = I;
				}
				if (_ && N0.jQuery === J) {
					N0.jQuery = C;
				}
				return J;
			},
			isReady : false,
			readyWait : 1,
			holdReady : function (_) {
				if (_) {
					J.readyWait++;
				} else {
					J.ready(true);
				}
			},
			ready : function (_) {
				if ((_ === true && !--J.readyWait) || (_ !== true && !J.isReady)) {
					if (!y0.body) {
						return setTimeout(J.ready, 1);
					}
					J.isReady = true;
					if (_ !== true && --J.readyWait > 0) {
						return;
					}
					W.fireWith(y0, [J]);
					if (J.fn.trigger) {
						J(y0).trigger("ready").off("ready");
					}
				}
			},
			bindReady : function () {
				if (W) {
					return;
				}
				W = J.Callbacks("once memory");
				if (y0.readyState === "complete") {
					return setTimeout(J.ready, 1);
				}
				if (y0.addEventListener) {
					y0.addEventListener("DOMContentLoaded", Y, false);
					N0.addEventListener("load", J.ready, false);
				} else {
					if (y0.attachEvent) {
						y0.attachEvent("onreadystatechange", Y);
						N0.attachEvent("onload", J.ready);
						var _ = false;
						try {
							_ = N0.frameElement == null;
						} catch (A) {}

						if (y0.documentElement.doScroll && _) {
							L();
						}
					}
				}
			},
			isFunction : function (_) {
				return J.type(_) === "function";
			},
			isArray : Array.isArray || function (_) {
				return J.type(_) === "array";
			},
			isWindow : function (_) {
				return _ != null && _ == _.window;
			},
			isNumeric : function (_) {
				return !isNaN(parseFloat(_)) && isFinite(_);
			},
			type : function (_) {
				return _ == null ? String(_) : Z[E.call(_)] || "object";
			},
			isPlainObject : function (A) {
				if (!A || J.type(A) !== "object" || A.nodeType || J.isWindow(A)) {
					return false;
				}
				try {
					if (A.constructor && !N.call(A, "constructor") && !N.call(A.constructor.prototype, "isPrototypeOf")) {
						return false;
					}
				} catch (B) {
					return false;
				}
				var _;
				for (_ in A) {}

				return _ === C1 || N.call(A, _);
			},
			isEmptyObject : function (_) {
				for (var A in _) {
					return false;
				}
				return true;
			},
			error : function (_) {
				throw new Error(_);
			},
			parseJSON : function (_) {
				if (typeof _ !== "string" || !_) {
					return null;
				}
				_ = J.trim(_);
				if (N0.JSON && N0.JSON.parse) {
					return N0.JSON.parse(_);
				}
				if (K.test(_.replace(d, "@").replace(T, "]").replace(a, ""))) {
					return (new Function("return " + _))();
				}
				J.error("Invalid JSON: " + _);
			},
			parseXML : function (A) {
				if (typeof A !== "string" || !A) {
					return null;
				}
				var B,
				_;
				try {
					if (N0.DOMParser) {
						_ = new DOMParser();
						B = _.parseFromString(A, "text/xml");
					} else {
						B = new ActiveXObject("Microsoft.XMLDOM");
						B.async = "false";
						B.loadXML(A);
					}
				} catch (C) {
					B = C1;
				}
				if (!B || !B.documentElement || B.getElementsByTagName("parsererror").length) {
					J.error("Invalid XML: " + A);
				}
				return B;
			},
			noop : function () {},
			globalEval : function (_) {
				if (_ && e.test(_)) {
					(N0.execScript || function (_) {
						N0.eval.call(N0, _);
					})(_);
				}
			},
			camelCase : function (_) {
				return _.replace(F, "ms-").replace(M, V);
			},
			nodeName : function (_, A) {
				return _.nodeName && _.nodeName.toUpperCase() === A.toUpperCase();
			},
			each : function (A, B, E) {
				var F,
				_ = 0,
				D = A.length,
				C = D === C1 || J.isFunction(A);
				if (E) {
					if (C) {
						for (F in A) {
							if (B.apply(A[F], E) === false) {
								break;
							}
						}
					} else {
						for (; _ < D; ) {
							if (B.apply(A[_++], E) === false) {
								break;
							}
						}
					}
				} else {
					if (C) {
						for (F in A) {
							if (B.call(A[F], F, A[F]) === false) {
								break;
							}
						}
					} else {
						for (; _ < D; ) {
							if (B.call(A[_], _, A[_++]) === false) {
								break;
							}
						}
					}
				}
				return A;
			},
			trim : D ? function (_) {
				return _ == null ? "" : D.call(_);
			}
			 : function (_) {
				return _ == null ? "" : _.toString().replace(Q, "").replace(S, "");
			},
			makeArray : function (C, B) {
				var A = B || [];
				if (C != null) {
					var _ = J.type(C);
					if (C.length == null || _ === "string" || _ === "function" || _ === "regexp" || J.isWindow(C)) {
						X.call(A, C);
					} else {
						J.merge(A, C);
					}
				}
				return A;
			},
			inArray : function (C, D, B) {
				var _;
				if (D) {
					if (A) {
						return A.call(D, C, B);
					}
					_ = D.length;
					B = B ? B < 0 ? Math.max(0, _ + B) : B : 0;
					for (; B < _; B++) {
						if (B in D && D[B] === C) {
							return B;
						}
					}
				}
				return -1;
			},
			merge : function (B, D) {
				var _ = B.length,
				A = 0;
				if (typeof D.length === "number") {
					for (var C = D.length; A < C; A++) {
						B[_++] = D[A];
					}
				} else {
					while (D[A] !== C1) {
						B[_++] = D[A++];
					}
				}
				B.length = _;
				return B;
			},
			grep : function (E, A, C) {
				var F = [],
				B;
				C = !!C;
				for (var _ = 0, D = E.length; _ < D; _++) {
					B = !!A(E[_], _);
					if (C !== B) {
						F.push(E[_]);
					}
				}
				return F;
			},
			map : function (F, D, B) {
				var C,
				_,
				G = [],
				A = 0,
				E = F.length,
				H = F instanceof J || E !== C1 && typeof E === "number" && ((E > 0 && F[0] && F[E - 1]) || E === 0 || J.isArray(F));
				if (H) {
					for (; A < E; A++) {
						C = D(F[A], A, B);
						if (C != null) {
							G[G.length] = C;
						}
					}
				} else {
					for (_ in F) {
						C = D(F[_], _, B);
						if (C != null) {
							G[G.length] = C;
						}
					}
				}
				return G.concat.apply([], G);
			},
			guid : 1,
			proxy : function (D, B) {
				if (typeof B === "string") {
					var _ = D[B];
					B = D;
					D = _;
				}
				if (!J.isFunction(D)) {
					return C1;
				}
				var A = R.call(arguments, 2),
				C = function () {
					return D.apply(B, A.concat(R.call(arguments)));
				};
				C.guid = D.guid = D.guid || C.guid || J.guid++;
				return C;
			},
			access : function (F, K, _, B, D, H, G) {
				var C,
				I = _ == null,
				A = 0,
				E = F.length;
				if (_ && typeof _ === "object") {
					for (A in _) {
						J.access(F, K, A, _[A], 1, H, B);
					}
					D = 1;
				} else {
					if (B !== C1) {
						C = G === C1 && J.isFunction(B);
						if (I) {
							if (C) {
								C = K;
								K = function (B, _, A) {
									return C.call(J(B), A);
								};
							} else {
								K.call(F, B);
								K = null;
							}
						}
						if (K) {
							for (; A < E; A++) {
								K(F[A], _, C ? B.call(F[A], A, K(F[A], _)) : B, G);
							}
						}
						D = 1;
					}
				}
				return D ? F : I ? K.call(F) : E ? K(F[0], _) : H;
			},
			now : function () {
				return (new Date()).getTime();
			},
			uaMatch : function (A) {
				A = A.toLowerCase();
				var _ = c.exec(A) || G.exec(A) || O.exec(A) || A.indexOf("compatible") < 0 && b.exec(A) || [];
				return {
					browser : _[1] || "",
					version : _[2] || "0"
				};
			},
			sub : function () {
				function A(_, B) {
					return new A.fn.init(_, B);
				}
				J.extend(true, A, this);
				A.superclass = this;
				A.fn = A.prototype = this();
				A.fn.constructor = A;
				A.sub = this.sub;
				A.fn.init = function _(C, D) {
					if (D && D instanceof J && !(D instanceof A)) {
						D = A(D);
					}
					return J.fn.init.call(this, C, D, B);
				};
				A.fn.init.prototype = A.fn;
				var B = A(y0);
				return A;
			},
			browser : {}

		});
		J.each("Boolean Number String Function Array Date RegExp Object".split(" "), function (_, A) {
			Z["[object " + A + "]"] = A.toLowerCase();
		});
		_ = J.uaMatch(U);
		if (_.browser) {
			J.browser[_.browser] = true;
			J.browser.version = _.version;
		}
		if (J.browser.webkit) {
			J.browser.safari = true;
		}
		if (e.test("\xa0")) {
			Q = /^[\s\xA0]+/;
			S = /[\s\xA0]+$/;
		}
		B = J(y0);
		if (y0.addEventListener) {
			Y = function () {
				y0.removeEventListener("DOMContentLoaded", Y, false);
				J.ready();
			};
		} else {
			if (y0.attachEvent) {
				Y = function () {
					if (y0.readyState === "complete") {
						y0.detachEvent("onreadystatechange", Y);
						J.ready();
					}
				};
			}
		}
		function L() {
			if (J.isReady) {
				return;
			}
			try {
				y0.documentElement.doScroll("left");
			} catch (_) {
				setTimeout(L, 1);
				return;
			}
			J.ready();
		}
		return J;
	})(),
	F0 = {};
	function r(C) {
		var A = F0[C] = {},
		_,
		B;
		C = C.split(/\s+/);
		for (_ = 0, B = C.length; _ < B; _++) {
			A[C[_]] = true;
		}
		return A;
	}
	n.Callbacks = function (I) {
		I = I ? (F0[I] || r(I)) : {};
		var _ = [],
		D = [],
		A,
		C,
		G,
		F,
		E,
		J,
		B = function (G) {
			var A,
			F,
			C,
			E,
			D;
			for (A = 0, F = G.length; A < F; A++) {
				C = G[A];
				E = n.type(C);
				if (E === "array") {
					B(C);
				} else {
					if (E === "function") {
						if (!I.unique || !H.has(C)) {
							_.push(C);
						}
					}
				}
			}
		},
		K = function (K, B) {
			B = B || [];
			A = !I.memory || [K, B];
			C = true;
			G = true;
			J = F || 0;
			F = 0;
			E = _.length;
			for (; _ && J < E; J++) {
				if (_[J].apply(K, B) === false && I.stopOnFalse) {
					A = true;
					break;
				}
			}
			G = false;
			if (_) {
				if (!I.once) {
					if (D && D.length) {
						A = D.shift();
						H.fireWith(A[0], A[1]);
					}
				} else {
					if (A === true) {
						H.disable();
					} else {
						_ = [];
					}
				}
			}
		},
		H = {
			add : function () {
				if (_) {
					var C = _.length;
					B(arguments);
					if (G) {
						E = _.length;
					} else {
						if (A && A !== true) {
							F = C;
							K(A[0], A[1]);
						}
					}
				}
				return this;
			},
			remove : function () {
				if (_) {
					var D = arguments,
					C = 0,
					B = D.length;
					for (; C < B; C++) {
						for (var A = 0; A < _.length; A++) {
							if (D[C] === _[A]) {
								if (G) {
									if (A <= E) {
										E--;
										if (A <= J) {
											J--;
										}
									}
								}
								_.splice(A--, 1);
								if (I.unique) {
									break;
								}
							}
						}
					}
				}
				return this;
			},
			has : function (C) {
				if (_) {
					var A = 0,
					B = _.length;
					for (; A < B; A++) {
						if (C === _[A]) {
							return true;
						}
					}
				}
				return false;
			},
			empty : function () {
				_ = [];
				return this;
			},
			disable : function () {
				_ = D = A = C1;
				return this;
			},
			disabled : function () {
				return !_;
			},
			lock : function () {
				D = C1;
				if (!A || A === true) {
					H.disable();
				}
				return this;
			},
			locked : function () {
				return !D;
			},
			fireWith : function (B, _) {
				if (D) {
					if (G) {
						if (!I.once) {
							D.push([B, _]);
						}
					} else {
						if (!(I.once && A)) {
							K(B, _);
						}
					}
				}
				return this;
			},
			fire : function () {
				H.fireWith(this, arguments);
				return this;
			},
			fired : function () {
				return !!C;
			}
		};
		return H;
	};
	var K = [].slice;
	n.extend({
		Deferred : function (E) {
			var B = n.Callbacks("once memory"),
			D = n.Callbacks("once memory"),
			G = n.Callbacks("memory"),
			F = "pending",
			H = {
				resolve : B,
				reject : D,
				notify : G
			},
			C = {
				done : B.add,
				fail : D.add,
				progress : G.add,
				state : function () {
					return F;
				},
				isResolved : B.fired,
				isRejected : D.fired,
				then : function (_, C, B) {
					A.done(_).fail(C).progress(B);
					return this;
				},
				always : function () {
					A.done.apply(A, arguments).fail.apply(A, arguments);
					return this;
				},
				pipe : function (_, C, B) {
					return n.Deferred(function (D) {
						n.each({
							done : [_, "resolve"],
							fail : [C, "reject"],
							progress : [B, "notify"]
						}, function (B, C) {
							var F = C[0],
							_ = C[1],
							E;
							if (n.isFunction(F)) {
								A[B](function () {
									E = F.apply(this, arguments);
									if (E && n.isFunction(E.promise)) {
										E.promise().then(D.resolve, D.reject, D.notify);
									} else {
										D[_ + "With"](this === A ? D : this, [E]);
									}
								});
							} else {
								A[B](D[_]);
							}
						});
					}).promise();
				},
				promise : function (A) {
					if (A == null) {
						A = C;
					} else {
						for (var _ in C) {
							A[_] = C[_];
						}
					}
					return A;
				}
			},
			A = C.promise({}),
			_;
			for (_ in H) {
				A[_] = H[_].fire;
				A[_ + "With"] = H[_].fireWith;
			}
			A.done(function () {
				F = "resolved";
			}, D.disable, G.lock).fail(function () {
				F = "rejected";
			}, B.disable, G.lock);
			if (E) {
				E.call(A, A);
			}
			return A;
		},
		when : function (D) {
			var F = K.call(arguments, 0),
			A = 0,
			E = F.length,
			J = new Array(E),
			I = E,
			G = E,
			_ = E <= 1 && D && n.isFunction(D.promise) ? D : n.Deferred(),
			C = _.promise();
			function B(A) {
				return function (B) {
					F[A] = arguments.length > 1 ? K.call(arguments, 0) : B;
					if (!(--I)) {
						_.resolveWith(_, F);
					}
				};
			}
			function H(A) {
				return function (B) {
					J[A] = arguments.length > 1 ? K.call(arguments, 0) : B;
					_.notifyWith(C, J);
				};
			}
			if (E > 1) {
				for (; A < E; A++) {
					if (F[A] && F[A].promise && n.isFunction(F[A].promise)) {
						F[A].promise().then(B(A), _.reject, H(A));
					} else {
						--I;
					}
				}
				if (!I) {
					_.resolveWith(_, F);
				}
			} else {
				if (_ !== D) {
					_.resolveWith(_, E ? [D] : []);
				}
			}
			return C;
		}
	});
	n.support = (function () {
		var L,
		J,
		H,
		D,
		K,
		A,
		I,
		B,
		_,
		M,
		E,
		F,
		G = y0.createElement("div"),
		C = y0.documentElement;
		G.setAttribute("className", "t");
		G.innerHTML = "   <link/><table></table><a href='/a' style='top:1px;float:left;opacity:.55;'>a</a><input type='checkbox'/>";
		J = G.getElementsByTagName("*");
		H = G.getElementsByTagName("a")[0];
		if (!J || !J.length || !H) {
			return {};
		}
		D = y0.createElement("select");
		K = D.appendChild(y0.createElement("option"));
		A = G.getElementsByTagName("input")[0];
		L = {
			leadingWhitespace : (G.firstChild.nodeType === 3),
			tbody : !G.getElementsByTagName("tbody").length,
			htmlSerialize : !!G.getElementsByTagName("link").length,
			style : /top/.test(H.getAttribute("style")),
			hrefNormalized : (H.getAttribute("href") === "/a"),
			opacity : /^0.55/.test(H.style.opacity),
			cssFloat : !!H.style.cssFloat,
			checkOn : (A.value === "on"),
			optSelected : K.selected,
			getSetAttribute : G.className !== "t",
			enctype : !!y0.createElement("form").enctype,
			html5Clone : y0.createElement("nav").cloneNode(true).outerHTML !== "<:nav></:nav>",
			submitBubbles : true,
			changeBubbles : true,
			focusinBubbles : false,
			deleteExpando : true,
			noCloneEvent : true,
			inlineBlockNeedsLayout : false,
			shrinkWrapBlocks : false,
			reliableMarginRight : true,
			pixelMargin : true
		};
		n.boxModel = L.boxModel = (y0.compatMode === "CSS1Compat");
		A.checked = true;
		L.noCloneChecked = A.cloneNode(true).checked;
		D.disabled = true;
		L.optDisabled = !K.disabled;
		try {
			delete G.test;
		} catch (N) {
			L.deleteExpando = false;
		}
		if (!G.addEventListener && G.attachEvent && G.fireEvent) {
			G.attachEvent("onclick", function () {
				L.noCloneEvent = false;
			});
			G.cloneNode(true).fireEvent("onclick");
		}
		A = y0.createElement("input");
		A.value = "t";
		A.setAttribute("type", "radio");
		L.radioValue = A.value === "t";
		A.setAttribute("checked", "checked");
		A.setAttribute("name", "t");
		G.appendChild(A);
		I = y0.createDocumentFragment();
		I.appendChild(G.lastChild);
		L.checkClone = I.cloneNode(true).cloneNode(true).lastChild.checked;
		L.appendChecked = A.checked;
		I.removeChild(A);
		I.appendChild(G);
		if (G.attachEvent) {
			for (E in {
				submit : 1,
				change : 1,
				focusin : 1
			}) {
				M = "on" + E;
				F = (M in G);
				if (!F) {
					G.setAttribute(M, "return;");
					F = (typeof G[M] === "function");
				}
				L[E + "Bubbles"] = F;
			}
		}
		I.removeChild(G);
		I = D = K = G = A = null;
		n(function () {
			var K,
			Q,
			D,
			_,
			I,
			C,
			N,
			J,
			M,
			E,
			H,
			A,
			P,
			O = y0.getElementsByTagName("body")[0];
			if (!O) {
				return;
			}
			J = 1;
			P = "padding:0;margin:0;border:";
			H = "position:absolute;top:0;left:0;width:1px;height:1px;";
			A = P + "0;visibility:hidden;";
			M = "style='" + H + P + "5px solid #000;";
			E = "<div " + M + "display:block;'><div style='" + P + "0;display:block;overflow:hidden;'></div></div>" + "<table " + M + "' cellpadding='0' cellspacing='0'>" + "<tr><td></td></tr></table>";
			K = y0.createElement("div");
			K.style.cssText = A + "width:0;height:0;position:static;top:0;margin-top:" + J + "px";
			O.insertBefore(K, O.firstChild);
			G = y0.createElement("div");
			K.appendChild(G);
			G.innerHTML = "<table><tr><td style='" + P + "0;display:none'></td><td>t</td></tr></table>";
			B = G.getElementsByTagName("td");
			F = (B[0].offsetHeight === 0);
			B[0].style.display = "";
			B[1].style.display = "none";
			L.reliableHiddenOffsets = F && (B[0].offsetHeight === 0);
			if (N0.getComputedStyle) {
				G.innerHTML = "";
				N = y0.createElement("div");
				N.style.width = "0";
				N.style.marginRight = "0";
				G.style.width = "2px";
				G.appendChild(N);
				L.reliableMarginRight = (parseInt((N0.getComputedStyle(N, null) || {
							marginRight : 0
						}).marginRight, 10) || 0) === 0;
			}
			if (typeof G.style.zoom !== "undefined") {
				G.innerHTML = "";
				G.style.width = G.style.padding = "1px";
				G.style.border = 0;
				G.style.overflow = "hidden";
				G.style.display = "inline";
				G.style.zoom = 1;
				L.inlineBlockNeedsLayout = (G.offsetWidth === 3);
				G.style.display = "block";
				G.style.overflow = "visible";
				G.innerHTML = "<div style='width:5px;'></div>";
				L.shrinkWrapBlocks = (G.offsetWidth !== 3);
			}
			G.style.cssText = H + A;
			G.innerHTML = E;
			Q = G.firstChild;
			D = Q.firstChild;
			I = Q.nextSibling.firstChild.firstChild;
			C = {
				doesNotAddBorder : (D.offsetTop !== 5),
				doesAddBorderForTableAndCells : (I.offsetTop === 5)
			};
			D.style.position = "fixed";
			D.style.top = "20px";
			C.fixedPosition = (D.offsetTop === 20 || D.offsetTop === 15);
			D.style.position = D.style.top = "";
			Q.style.overflow = "hidden";
			Q.style.position = "relative";
			C.subtractsBorderForOverflowNotVisible = (D.offsetTop === -5);
			C.doesNotIncludeMarginInBodyOffset = (O.offsetTop !== J);
			if (N0.getComputedStyle) {
				G.style.marginTop = "1%";
				L.pixelMargin = (N0.getComputedStyle(G, null) || {
					marginTop : 0
				}).marginTop !== "1%";
			}
			if (typeof K.style.zoom !== "undefined") {
				K.style.zoom = 1;
			}
			O.removeChild(K);
			N = G = K = null;
			n.extend(L, C);
		});
		return L;
	})();
	var A1 = /^(?:\{.*\}|\[.*\])$/,
	G1 = /([A-Z])/g;
	n.extend({
		cache : {},
		uuid : 0,
		expando : "jQuery" + (n.fn.jquery + Math.random()).replace(/\D/g, ""),
		noData : {
			embed : true,
			object : "clsid:D27CDB6E-AE6D-11cf-96B8-444553540000",
			applet : true
		},
		hasData : function (_) {
			_ = _.nodeType ? n.cache[_[n.expando]] : _[n.expando];
			return !!_ && !w0(_);
		},
		data : function (A, K, H, _) {
			if (!n.acceptData(A)) {
				return;
			}
			var J,
			B,
			I,
			G = n.expando,
			D = typeof K === "string",
			C = A.nodeType,
			F = C ? n.cache : A,
			E = C ? A[G] : A[G] && G,
			L = K === "events";
			if ((!E || !F[E] || (!L && !_ && !F[E].data)) && D && H === C1) {
				return;
			}
			if (!E) {
				if (C) {
					A[G] = E = ++n.uuid;
				} else {
					E = G;
				}
			}
			if (!F[E]) {
				F[E] = {};
				if (!C) {
					F[E].toJSON = n.noop;
				}
			}
			if (typeof K === "object" || typeof K === "function") {
				if (_) {
					F[E] = n.extend(F[E], K);
				} else {
					F[E].data = n.extend(F[E].data, K);
				}
			}
			J = B = F[E];
			if (!_) {
				if (!B.data) {
					B.data = {};
				}
				B = B.data;
			}
			if (H !== C1) {
				B[n.camelCase(K)] = H;
			}
			if (L && !B[K]) {
				return J.events;
			}
			if (D) {
				I = B[K];
				if (I == null) {
					I = B[n.camelCase(K)];
				}
			} else {
				I = B;
			}
			return I;
		},
		removeData : function (D, H, B) {
			if (!n.acceptData(D)) {
				return;
			}
			var C,
			A,
			G,
			F = n.expando,
			E = D.nodeType,
			_ = E ? n.cache : D,
			I = E ? D[F] : F;
			if (!_[I]) {
				return;
			}
			if (H) {
				C = B ? _[I] : _[I].data;
				if (C) {
					if (!n.isArray(H)) {
						if (H in C) {
							H = [H];
						} else {
							H = n.camelCase(H);
							if (H in C) {
								H = [H];
							} else {
								H = H.split(" ");
							}
						}
					}
					for (A = 0, G = H.length; A < G; A++) {
						delete C[H[A]];
					}
					if (!(B ? w0 : n.isEmptyObject)(C)) {
						return;
					}
				}
			}
			if (!B) {
				delete _[I].data;
				if (!w0(_[I])) {
					return;
				}
			}
			if (n.support.deleteExpando || !_.setInterval) {
				delete _[I];
			} else {
				_[I] = null;
			}
			if (E) {
				if (n.support.deleteExpando) {
					delete D[F];
				} else {
					if (D.removeAttribute) {
						D.removeAttribute(F);
					} else {
						D[F] = null;
					}
				}
			}
		},
		_data : function (_, B, A) {
			return n.data(_, B, A, true);
		},
		acceptData : function (A) {
			if (A.nodeName) {
				var _ = n.noData[A.nodeName.toLowerCase()];
				if (_) {
					return !(_ === true || A.getAttribute("classid") !== _);
				}
			}
			return true;
		}
	});
	n.fn.extend({
		data : function (A, D) {
			var _,
			E,
			G,
			I,
			H,
			C = this[0],
			B = 0,
			F = null;
			if (A === C1) {
				if (this.length) {
					F = n.data(C);
					if (C.nodeType === 1 && !n._data(C, "parsedAttrs")) {
						G = C.attributes;
						for (H = G.length; B < H; B++) {
							I = G[B].name;
							if (I.indexOf("data-") === 0) {
								I = n.camelCase(I.substring(5));
								b0(C, I, F[I]);
							}
						}
						n._data(C, "parsedAttrs", true);
					}
				}
				return F;
			}
			if (typeof A === "object") {
				return this.each(function () {
					n.data(this, A);
				});
			}
			_ = A.split(".", 2);
			_[1] = _[1] ? "." + _[1] : "";
			E = _[1] + "!";
			return n.access(this, function (B) {
				if (B === C1) {
					F = this.triggerHandler("getData" + E, [_[0]]);
					if (F === C1 && C) {
						F = n.data(C, A);
						F = b0(C, A, F);
					}
					return F === C1 && _[1] ? this.data(_[0]) : F;
				}
				_[1] = B;
				this.each(function () {
					var C = n(this);
					C.triggerHandler("setData" + E, _);
					n.data(this, A, B);
					C.triggerHandler("changeData" + E, _);
				});
			}, null, D, arguments.length > 1, null, false);
		},
		removeData : function (_) {
			return this.each(function () {
				n.removeData(this, _);
			});
		}
	});
	function b0(A, _, B) {
		if (B === C1 && A.nodeType === 1) {
			var C = "data-" + _.replace(G1, "-$1").toLowerCase();
			B = A.getAttribute(C);
			if (typeof B === "string") {
				try {
					B = B === "true" ? true : B === "false" ? false : B === "null" ? null : n.isNumeric(B) ? +B : A1.test(B) ? n.parseJSON(B) : B;
				} catch (D) {}

				n.data(A, _, B);
			} else {
				B = C1;
			}
		}
		return B;
	}
	function w0(_) {
		for (var A in _) {
			if (A === "data" && n.isEmptyObject(_[A])) {
				continue;
			}
			if (A !== "toJSON") {
				return false;
			}
		}
		return true;
	}
	function G(_, A, C) {
		var F = A + "defer",
		D = A + "queue",
		B = A + "mark",
		E = n._data(_, F);
		if (E && (C === "queue" || !n._data(_, D)) && (C === "mark" || !n._data(_, B))) {
			setTimeout(function () {
				if (!n._data(_, D) && !n._data(_, B)) {
					n.removeData(_, F, true);
					E.fire();
				}
			}, 0);
		}
	}
	n.extend({
		_mark : function (_, A) {
			if (_) {
				A = (A || "fx") + "mark";
				n._data(_, A, (n._data(_, A) || 0) + 1);
			}
		},
		_unmark : function (C, A, B) {
			if (C !== true) {
				B = A;
				A = C;
				C = false;
			}
			if (A) {
				B = B || "fx";
				var _ = B + "mark",
				D = C ? 0 : ((n._data(A, _) || 1) - 1);
				if (D) {
					n._data(A, _, D);
				} else {
					n.removeData(A, _, true);
					G(A, B, "mark");
				}
			}
		},
		queue : function (_, A, B) {
			var C;
			if (_) {
				A = (A || "fx") + "queue";
				C = n._data(_, A);
				if (B) {
					if (!C || n.isArray(B)) {
						C = n._data(_, A, n.makeArray(B));
					} else {
						C.push(B);
					}
				}
				return C || [];
			}
		},
		dequeue : function (_, A) {
			A = A || "fx";
			var C = n.queue(_, A),
			D = C.shift(),
			B = {};
			if (D === "inprogress") {
				D = C.shift();
			}
			if (D) {
				if (A === "fx") {
					C.unshift("inprogress");
				}
				n._data(_, A + ".run", B);
				D.call(_, function () {
					n.dequeue(_, A);
				}, B);
			}
			if (!C.length) {
				n.removeData(_, A + "queue " + A + ".run", true);
				G(_, A, "queue");
			}
		}
	});
	n.fn.extend({
		queue : function (_, B) {
			var A = 2;
			if (typeof _ !== "string") {
				B = _;
				_ = "fx";
				A--;
			}
			if (arguments.length < A) {
				return n.queue(this[0], _);
			}
			return B === C1 ? this : this.each(function () {
				var A = n.queue(this, _, B);
				if (_ === "fx" && A[0] !== "inprogress") {
					n.dequeue(this, _);
				}
			});
		},
		dequeue : function (_) {
			return this.each(function () {
				n.dequeue(this, _);
			});
		},
		delay : function (A, _) {
			A = n.fx ? n.fx.speeds[A] || A : A;
			_ = _ || "fx";
			return this.queue(_, function (C, B) {
				var _ = setTimeout(C, A);
				B.stop = function () {
					clearTimeout(_);
				};
			});
		},
		clearQueue : function (_) {
			return this.queue(_ || "fx", []);
		},
		promise : function (C, A) {
			if (typeof C !== "string") {
				A = C;
				C = C1;
			}
			C = C || "fx";
			var J = n.Deferred(),
			I = this,
			_ = I.length,
			F = 1,
			H = C + "defer",
			G = C + "queue",
			D = C + "mark",
			B;
			function E() {
				if (!(--F)) {
					J.resolveWith(I, [I]);
				}
			}
			while (_--) {
				if ((B = n.data(I[_], H, C1, true) || (n.data(I[_], G, C1, true) || n.data(I[_], D, C1, true)) && n.data(I[_], H, n.Callbacks("once memory"), true))) {
					F++;
					B.add(E);
				}
			}
			E();
			return J.promise(A);
		}
	});
	var T = /[\n\t\r]/g,
	i0 = /\s+/,
	O1 = /\r/g,
	X1 = /^(?:button|input)$/i,
	k0 = /^(?:button|input|object|select|textarea)$/i,
	E0 = /^a(?:rea)?$/i,
	_0 = /^(?:autofocus|autoplay|async|checked|controls|defer|disabled|hidden|loop|multiple|open|readonly|required|scoped|selected)$/i,
	q0 = n.support.getSetAttribute,
	N,
	l0,
	h;
	n.fn.extend({
		attr : function (A, _) {
			return n.access(this, n.attr, A, _, arguments.length > 1);
		},
		removeAttr : function (_) {
			return this.each(function () {
				n.removeAttr(this, _);
			});
		},
		prop : function (A, _) {
			return n.access(this, n.prop, A, _, arguments.length > 1);
		},
		removeProp : function (_) {
			_ = n.propFix[_] || _;
			return this.each(function () {
				try {
					this[_] = C1;
					delete this[_];
				} catch (A) {}

			});
		},
		addClass : function (D) {
			var C,
			_,
			F,
			B,
			G,
			E,
			A;
			if (n.isFunction(D)) {
				return this.each(function (_) {
					n(this).addClass(D.call(this, _, this.className));
				});
			}
			if (D && typeof D === "string") {
				C = D.split(i0);
				for (_ = 0, F = this.length; _ < F; _++) {
					B = this[_];
					if (B.nodeType === 1) {
						if (!B.className && C.length === 1) {
							B.className = D;
						} else {
							G = " " + B.className + " ";
							for (E = 0, A = C.length; E < A; E++) {
								if (!~G.indexOf(" " + C[E] + " ")) {
									G += C[E] + " ";
								}
							}
							B.className = n.trim(G);
						}
					}
				}
			}
			return this;
		},
		removeClass : function (D) {
			var C,
			_,
			G,
			B,
			F,
			E,
			A;
			if (n.isFunction(D)) {
				return this.each(function (_) {
					n(this).removeClass(D.call(this, _, this.className));
				});
			}
			if ((D && typeof D === "string") || D === C1) {
				C = (D || "").split(i0);
				for (_ = 0, G = this.length; _ < G; _++) {
					B = this[_];
					if (B.nodeType === 1 && B.className) {
						if (D) {
							F = (" " + B.className + " ").replace(T, " ");
							for (E = 0, A = C.length; E < A; E++) {
								F = F.replace(" " + C[E] + " ", " ");
							}
							B.className = n.trim(F);
						} else {
							B.className = "";
						}
					}
				}
			}
			return this;
		},
		toggleClass : function (_, B) {
			var A = typeof _,
			C = typeof B === "boolean";
			if (n.isFunction(_)) {
				return this.each(function (A) {
					n(this).toggleClass(_.call(this, A, this.className, B), B);
				});
			}
			return this.each(function () {
				if (A === "string") {
					var H,
					D = 0,
					F = n(this),
					G = B,
					E = _.split(i0);
					while ((H = E[D++])) {
						G = C ? G : !F.hasClass(H);
						F[G ? "addClass" : "removeClass"](H);
					}
				} else {
					if (A === "undefined" || A === "boolean") {
						if (this.className) {
							n._data(this, "__className__", this.className);
						}
						this.className = this.className || _ === false ? "" : n._data(this, "__className__") || "";
					}
				}
			});
		},
		hasClass : function (A) {
			var B = " " + A + " ",
			_ = 0,
			C = this.length;
			for (; _ < C; _++) {
				if (this[_].nodeType === 1 && (" " + this[_].className + " ").replace(T, " ").indexOf(B) > -1) {
					return true;
				}
			}
			return false;
		},
		val : function (A) {
			var B,
			C,
			D,
			_ = this[0];
			if (!arguments.length) {
				if (_) {
					B = n.valHooks[_.type] || n.valHooks[_.nodeName.toLowerCase()];
					if (B && "get" in B && (C = B.get(_, "value")) !== C1) {
						return C;
					}
					C = _.value;
					return typeof C === "string" ? C.replace(O1, "") : C == null ? "" : C;
				}
				return;
			}
			D = n.isFunction(A);
			return this.each(function (C) {
				var E = n(this),
				_;
				if (this.nodeType !== 1) {
					return;
				}
				if (D) {
					_ = A.call(this, C, E.val());
				} else {
					_ = A;
				}
				if (_ == null) {
					_ = "";
				} else {
					if (typeof _ === "number") {
						_ += "";
					} else {
						if (n.isArray(_)) {
							_ = n.map(_, function (_) {
									return _ == null ? "" : _ + "";
								});
						}
					}
				}
				B = n.valHooks[this.type] || n.valHooks[this.nodeName.toLowerCase()];
				if (!B || !("set" in B) || B.set(this, _, "value") === C1) {
					this.value = _;
				}
			});
		}
	});
	n.extend({
		valHooks : {
			option : {
				get : function (A) {
					var _ = A.attributes.value;
					return !_ || _.specified ? A.value : A.text;
				}
			},
			select : {
				get : function (D) {
					var C,
					B,
					G,
					E,
					_ = D.selectedIndex,
					F = [],
					H = D.options,
					A = D.type === "select-one";
					if (_ < 0) {
						return null;
					}
					B = A ? _ : 0;
					G = A ? _ + 1 : H.length;
					for (; B < G; B++) {
						E = H[B];
						if (E.selected && (n.support.optDisabled ? !E.disabled : E.getAttribute("disabled") === null) && (!E.parentNode.disabled || !n.nodeName(E.parentNode, "optgroup"))) {
							C = n(E).val();
							if (A) {
								return C;
							}
							F.push(C);
						}
					}
					if (A && !F.length && H.length) {
						return n(H[_]).val();
					}
					return F;
				},
				set : function (A, _) {
					var B = n.makeArray(_);
					n(A).find("option").each(function () {
						this.selected = n.inArray(n(this).val(), B) >= 0;
					});
					if (!B.length) {
						A.selectedIndex = -1;
					}
					return B;
				}
			}
		},
		attrFn : {
			val : true,
			css : true,
			html : true,
			text : true,
			data : true,
			width : true,
			height : true,
			offset : true
		},
		attr : function (A, G, _, D) {
			var E,
			B,
			C,
			F = A.nodeType;
			if (!A || F === 3 || F === 8 || F === 2) {
				return;
			}
			if (D && G in n.attrFn) {
				return n(A)[G](_);
			}
			if (typeof A.getAttribute === "undefined") {
				return n.prop(A, G, _);
			}
			C = F !== 1 || !n.isXMLDoc(A);
			if (C) {
				G = G.toLowerCase();
				B = n.attrHooks[G] || (_0.test(G) ? l0 : N);
			}
			if (_ !== C1) {
				if (_ === null) {
					n.removeAttr(A, G);
					return;
				} else {
					if (B && "set" in B && C && (E = B.set(A, _, G)) !== C1) {
						return E;
					} else {
						A.setAttribute(G, "" + _);
						return _;
					}
				}
			} else {
				if (B && "get" in B && C && (E = B.get(A, G)) !== null) {
					return E;
				} else {
					E = A.getAttribute(G);
					return E === null ? C1 : E;
				}
			}
		},
		removeAttr : function (B, A) {
			var D,
			C,
			F,
			E,
			G,
			_ = 0;
			if (A && B.nodeType === 1) {
				C = A.toLowerCase().split(i0);
				E = C.length;
				for (; _ < E; _++) {
					F = C[_];
					if (F) {
						D = n.propFix[F] || F;
						G = _0.test(F);
						if (!G) {
							n.attr(B, F, "");
						}
						B.removeAttribute(q0 ? F : D);
						if (G && D in B) {
							B[D] = false;
						}
					}
				}
			}
		},
		attrHooks : {
			type : {
				set : function (B, A) {
					if (X1.test(B.nodeName) && B.parentNode) {
						n.error("type property can't be changed");
					} else {
						if (!n.support.radioValue && A === "radio" && n.nodeName(B, "input")) {
							var _ = B.value;
							B.setAttribute("type", A);
							if (_) {
								B.value = _;
							}
							return A;
						}
					}
				}
			},
			value : {
				get : function (_, A) {
					if (N && n.nodeName(_, "button")) {
						return N.get(_, A);
					}
					return A in _ ? _.value : null;
				},
				set : function (A, _, B) {
					if (N && n.nodeName(A, "button")) {
						return N.set(A, _, B);
					}
					A.value = _;
				}
			}
		},
		propFix : {
			tabindex : "tabIndex",
			readonly : "readOnly",
			"for" : "htmlFor",
			"class" : "className",
			maxlength : "maxLength",
			cellspacing : "cellSpacing",
			cellpadding : "cellPadding",
			rowspan : "rowSpan",
			colspan : "colSpan",
			usemap : "useMap",
			frameborder : "frameBorder",
			contenteditable : "contentEditable"
		},
		prop : function (A, F, _) {
			var D,
			B,
			C,
			E = A.nodeType;
			if (!A || E === 3 || E === 8 || E === 2) {
				return;
			}
			C = E !== 1 || !n.isXMLDoc(A);
			if (C) {
				F = n.propFix[F] || F;
				B = n.propHooks[F];
			}
			if (_ !== C1) {
				if (B && "set" in B && (D = B.set(A, _, F)) !== C1) {
					return D;
				} else {
					return (A[F] = _);
				}
			} else {
				if (B && "get" in B && (D = B.get(A, F)) !== null) {
					return D;
				} else {
					return A[F];
				}
			}
		},
		propHooks : {
			tabIndex : {
				get : function (A) {
					var _ = A.getAttributeNode("tabindex");
					return _ && _.specified ? parseInt(_.value, 10) : k0.test(A.nodeName) || E0.test(A.nodeName) && A.href ? 0 : C1;
				}
			}
		}
	});
	n.attrHooks.tabindex = n.propHooks.tabIndex;
	l0 = {
		get : function (A, C) {
			var _,
			B = n.prop(A, C);
			return B === true || typeof B !== "boolean" && (_ = A.getAttributeNode(C)) && _.nodeValue !== false ? C.toLowerCase() : C1;
		},
		set : function (A, _, C) {
			var B;
			if (_ === false) {
				n.removeAttr(A, C);
			} else {
				B = n.propFix[C] || C;
				if (B in A) {
					A[B] = true;
				}
				A.setAttribute(C, C.toLowerCase());
			}
			return C;
		}
	};
	if (!q0) {
		h = {
			name : true,
			id : true,
			coords : true
		};
		N = n.valHooks.button = {
			get : function (_, B) {
				var A;
				A = _.getAttributeNode(B);
				return A && (h[B] ? A.nodeValue !== "" : A.specified) ? A.nodeValue : C1;
			},
			set : function (A, _, C) {
				var B = A.getAttributeNode(C);
				if (!B) {
					B = y0.createAttribute(C);
					A.setAttributeNode(B);
				}
				return (B.nodeValue = _ + "");
			}
		};
		n.attrHooks.tabindex.set = N.set;
		n.each(["width", "height"], function (_, A) {
			n.attrHooks[A] = n.extend(n.attrHooks[A], {
					set : function (B, _) {
						if (_ === "") {
							B.setAttribute(A, "auto");
							return _;
						}
					}
				});
		});
		n.attrHooks.contenteditable = {
			get : N.get,
			set : function (A, _, B) {
				if (_ === "") {
					_ = "false";
				}
				N.set(A, _, B);
			}
		};
	}
	if (!n.support.hrefNormalized) {
		n.each(["href", "src", "width", "height"], function (_, A) {
			n.attrHooks[A] = n.extend(n.attrHooks[A], {
					get : function (_) {
						var B = _.getAttribute(A, 2);
						return B === null ? C1 : B;
					}
				});
		});
	}
	if (!n.support.style) {
		n.attrHooks.style = {
			get : function (_) {
				return _.style.cssText.toLowerCase() || C1;
			},
			set : function (A, _) {
				return (A.style.cssText = "" + _);
			}
		};
	}
	if (!n.support.optSelected) {
		n.propHooks.selected = n.extend(n.propHooks.selected, {
				get : function (_) {
					var A = _.parentNode;
					if (A) {
						A.selectedIndex;
						if (A.parentNode) {
							A.parentNode.selectedIndex;
						}
					}
					return null;
				}
			});
	}
	if (!n.support.enctype) {
		n.propFix.enctype = "encoding";
	}
	if (!n.support.checkOn) {
		n.each(["radio", "checkbox"], function () {
			n.valHooks[this] = {
				get : function (_) {
					return _.getAttribute("value") === null ? "on" : _.value;
				}
			};
		});
	}
	n.each(["radio", "checkbox"], function () {
		n.valHooks[this] = n.extend(n.valHooks[this], {
				set : function (A, _) {
					if (n.isArray(_)) {
						return (A.checked = n.inArray(n(A).val(), _) >= 0);
					}
				}
			});
	});
	var s0 = /^(?:textarea|input|select)$/i,
	W1 = /^([^\.]*)?(?:\.(.+))?$/,
	I1 = /(?:^|\s)hover(\.\S+)?\b/,
	L = /^key/,
	E1 = /^(?:mouse|contextmenu)|click/,
	j = /^(?:focusinfocus|focusoutblur)$/,
	B0 = /^(\w*)(?:#([\w\-]+))?(?:\.([\w\-]+))?$/,
	Z = function (_) {
		var A = B0.exec(_);
		if (A) {
			A[1] = (A[1] || "").toLowerCase();
			A[3] = A[3] && new RegExp("(?:^|\\s)" + A[3] + "(?:\\s|$)");
		}
		return A;
	},
	Q0 = function (_, A) {
		var B = _.attributes || {};
		return ((!A[1] || _.nodeName.toLowerCase() === A[1]) && (!A[2] || (B.id || {}).value === A[2]) && (!A[3] || A[3].test((B["class"] || {}).value)));
	},
	d = function (_) {
		return n.event.special.hover ? _ : _.replace(I1, "mouseenter$1 mouseleave$1");
	};
	n.event = {
		add : function (B, D, C, O, F) {
			var L,
			E,
			A,
			P,
			_,
			H,
			K,
			J,
			N,
			I,
			M,
			G;
			if (B.nodeType === 3 || B.nodeType === 8 || !D || !C || !(L = n._data(B))) {
				return;
			}
			if (C.handler) {
				N = C;
				C = N.handler;
				F = N.selector;
			}
			if (!C.guid) {
				C.guid = n.guid++;
			}
			A = L.events;
			if (!A) {
				L.events = A = {};
			}
			E = L.handle;
			if (!E) {
				L.handle = E = function (_) {
					return typeof n !== "undefined" && (!_ || n.event.triggered !== _.type) ? n.event.dispatch.apply(E.elem, arguments) : C1;
				};
				E.elem = B;
			}
			D = n.trim(d(D)).split(" ");
			for (P = 0; P < D.length; P++) {
				_ = W1.exec(D[P]) || [];
				H = _[1];
				K = (_[2] || "").split(".").sort();
				G = n.event.special[H] || {};
				H = (F ? G.delegateType : G.bindType) || H;
				G = n.event.special[H] || {};
				J = n.extend({
						type : H,
						origType : _[1],
						data : O,
						handler : C,
						guid : C.guid,
						selector : F,
						quick : F && Z(F),
						namespace : K.join(".")
					}, N);
				M = A[H];
				if (!M) {
					M = A[H] = [];
					M.delegateCount = 0;
					if (!G.setup || G.setup.call(B, O, K, E) === false) {
						if (B.addEventListener) {
							B.addEventListener(H, E, false);
						} else {
							if (B.attachEvent) {
								B.attachEvent("on" + H, E);
							}
						}
					}
				}
				if (G.add) {
					G.add.call(B, J);
					if (!J.handler.guid) {
						J.handler.guid = C.guid;
					}
				}
				if (F) {
					M.splice(M.delegateCount++, 0, J);
				} else {
					M.push(J);
				}
				n.event.global[H] = true;
			}
			B = null;
		},
		global : {},
		remove : function (D, F, E, G, H) {
			var M = n.hasData(D) && n._data(D),
			N,
			A,
			J,
			P,
			L,
			B,
			O,
			C,
			I,
			_,
			Q,
			K;
			if (!M || !(C = M.events)) {
				return;
			}
			F = n.trim(d(F || "")).split(" ");
			for (N = 0; N < F.length; N++) {
				A = W1.exec(F[N]) || [];
				J = P = A[1];
				L = A[2];
				if (!J) {
					for (J in C) {
						n.event.remove(D, J + F[N], E, G, true);
					}
					continue;
				}
				I = n.event.special[J] || {};
				J = (G ? I.delegateType : I.bindType) || J;
				Q = C[J] || [];
				B = Q.length;
				L = L ? new RegExp("(^|\\.)" + L.split(".").sort().join("\\.(?:.*\\.)?") + "(\\.|$)") : null;
				for (O = 0; O < Q.length; O++) {
					K = Q[O];
					if ((H || P === K.origType) && (!E || E.guid === K.guid) && (!L || L.test(K.namespace)) && (!G || G === K.selector || G === "**" && K.selector)) {
						Q.splice(O--, 1);
						if (K.selector) {
							Q.delegateCount--;
						}
						if (I.remove) {
							I.remove.call(D, K);
						}
					}
				}
				if (Q.length === 0 && B !== Q.length) {
					if (!I.teardown || I.teardown.call(D, L) === false) {
						n.removeEvent(D, J, M.handle);
					}
					delete C[J];
				}
			}
			if (n.isEmptyObject(C)) {
				_ = M.handle;
				if (_) {
					_.elem = null;
				}
				n.removeData(D, ["events", "handle"], true);
			}
		},
		customEvent : {
			getData : true,
			setData : true,
			changeData : true
		},
		trigger : function (M, L, A, K) {
			if (A && (A.nodeType === 3 || A.nodeType === 8)) {
				return;
			}
			var C = M.type || M,
			G = [],
			H,
			F,
			I,
			E,
			J,
			O,
			B,
			_,
			N,
			D;
			if (j.test(C + n.event.triggered)) {
				return;
			}
			if (C.indexOf("!") >= 0) {
				C = C.slice(0, -1);
				F = true;
			}
			if (C.indexOf(".") >= 0) {
				G = C.split(".");
				C = G.shift();
				G.sort();
			}
			if ((!A || n.event.customEvent[C]) && !n.event.global[C]) {
				return;
			}
			M = typeof M === "object" ? M[n.expando] ? M : new n.Event(C, M) : new n.Event(C);
			M.type = C;
			M.isTrigger = true;
			M.exclusive = F;
			M.namespace = G.join(".");
			M.namespace_re = M.namespace ? new RegExp("(^|\\.)" + G.join("\\.(?:.*\\.)?") + "(\\.|$)") : null;
			O = C.indexOf(":") < 0 ? "on" + C : "";
			if (!A) {
				H = n.cache;
				for (I in H) {
					if (H[I].events && H[I].events[C]) {
						n.event.trigger(M, L, H[I].handle.elem, true);
					}
				}
				return;
			}
			M.result = C1;
			if (!M.target) {
				M.target = A;
			}
			L = L != null ? n.makeArray(L) : [];
			L.unshift(M);
			B = n.event.special[C] || {};
			if (B.trigger && B.trigger.apply(A, L) === false) {
				return;
			}
			N = [[A, B.bindType || C]];
			if (!K && !B.noBubble && !n.isWindow(A)) {
				D = B.delegateType || C;
				E = j.test(D + C) ? A : A.parentNode;
				J = null;
				for (; E; E = E.parentNode) {
					N.push([E, D]);
					J = E;
				}
				if (J && J === A.ownerDocument) {
					N.push([J.defaultView || J.parentWindow || N0, D]);
				}
			}
			for (I = 0; I < N.length && !M.isPropagationStopped(); I++) {
				E = N[I][0];
				M.type = N[I][1];
				_ = (n._data(E, "events") || {})[M.type] && n._data(E, "handle");
				if (_) {
					_.apply(E, L);
				}
				_ = O && E[O];
				if (_ && n.acceptData(E) && _.apply(E, L) === false) {
					M.preventDefault();
				}
			}
			M.type = C;
			if (!K && !M.isDefaultPrevented()) {
				if ((!B._default || B._default.apply(A.ownerDocument, L) === false) && !(C === "click" && n.nodeName(A, "a")) && n.acceptData(A)) {
					if (O && A[C] && ((C !== "focus" && C !== "blur") || M.target.offsetWidth !== 0) && !n.isWindow(A)) {
						J = A[O];
						if (J) {
							A[O] = null;
						}
						n.event.triggered = C;
						A[C]();
						n.event.triggered = C1;
						if (J) {
							A[O] = J;
						}
					}
				}
			}
			return M.result;
		},
		dispatch : function (L) {
			L = n.event.fix(L || N0.event);
			var J = ((n._data(this, "events") || {})[L.type] || []),
			P = J.delegateCount,
			C = [].slice.call(arguments, 0),
			Q = !L.exclusive && !L.namespace,
			B = n.event.special[L.type] || {},
			F = [],
			I,
			M,
			D,
			G,
			O,
			A,
			K,
			_,
			E,
			N,
			H;
			C[0] = L;
			L.delegateTarget = this;
			if (B.preDispatch && B.preDispatch.call(this, L) === false) {
				return;
			}
			if (P && !(L.button && L.type === "click")) {
				G = n(this);
				G.context = this.ownerDocument || this;
				for (D = L.target; D != this; D = D.parentNode || this) {
					if (D.disabled !== true) {
						A = {};
						_ = [];
						G[0] = D;
						for (I = 0; I < P; I++) {
							E = J[I];
							N = E.selector;
							if (A[N] === C1) {
								A[N] = (E.quick ? Q0(D, E.quick) : G.is(N));
							}
							if (A[N]) {
								_.push(E);
							}
						}
						if (_.length) {
							F.push({
								elem : D,
								matches : _
							});
						}
					}
				}
			}
			if (J.length > P) {
				F.push({
					elem : this,
					matches : J.slice(P)
				});
			}
			for (I = 0; I < F.length && !L.isPropagationStopped(); I++) {
				K = F[I];
				L.currentTarget = K.elem;
				for (M = 0; M < K.matches.length && !L.isImmediatePropagationStopped(); M++) {
					E = K.matches[M];
					if (Q || (!L.namespace && !E.namespace) || L.namespace_re && L.namespace_re.test(E.namespace)) {
						L.data = E.data;
						L.handleObj = E;
						O = ((n.event.special[E.origType] || {}).handle || E.handler).apply(K.elem, C);
						if (O !== C1) {
							L.result = O;
							if (O === false) {
								L.preventDefault();
								L.stopPropagation();
							}
						}
					}
				}
			}
			if (B.postDispatch) {
				B.postDispatch.call(this, L);
			}
			return L.result;
		},
		props : "attrChange attrName relatedNode srcElement altKey bubbles cancelable ctrlKey currentTarget eventPhase metaKey relatedTarget shiftKey target timeStamp view which".split(" "),
		fixHooks : {},
		keyHooks : {
			props : "char charCode key keyCode".split(" "),
			filter : function (A, _) {
				if (A.which == null) {
					A.which = _.charCode != null ? _.charCode : _.keyCode;
				}
				return A;
			}
		},
		mouseHooks : {
			props : "button buttons clientX clientY fromElement offsetX offsetY pageX pageY screenX screenY toElement".split(" "),
			filter : function (C, B) {
				var A,
				_,
				F,
				E = B.button,
				D = B.fromElement;
				if (C.pageX == null && B.clientX != null) {
					A = C.target.ownerDocument || y0;
					_ = A.documentElement;
					F = A.body;
					C.pageX = B.clientX + (_ && _.scrollLeft || F && F.scrollLeft || 0) - (_ && _.clientLeft || F && F.clientLeft || 0);
					C.pageY = B.clientY + (_ && _.scrollTop || F && F.scrollTop || 0) - (_ && _.clientTop || F && F.clientTop || 0);
				}
				if (!C.relatedTarget && D) {
					C.relatedTarget = D === C.target ? B.toElement : D;
				}
				if (!C.which && E !== C1) {
					C.which = (E & 1 ? 1 : (E & 2 ? 3 : (E & 4 ? 2 : 0)));
				}
				return C;
			}
		},
		fix : function (D) {
			if (D[n.expando]) {
				return D;
			}
			var _,
			A,
			C = D,
			E = n.event.fixHooks[D.type] || {},
			B = E.props ? this.props.concat(E.props) : this.props;
			D = n.Event(C);
			for (_ = B.length; _; ) {
				A = B[--_];
				D[A] = C[A];
			}
			if (!D.target) {
				D.target = C.srcElement || y0;
			}
			if (D.target.nodeType === 3) {
				D.target = D.target.parentNode;
			}
			if (D.metaKey === C1) {
				D.metaKey = D.ctrlKey;
			}
			return E.filter ? E.filter(D, C) : D;
		},
		special : {
			ready : {
				setup : n.bindReady
			},
			load : {
				noBubble : true
			},
			focus : {
				delegateType : "focusin"
			},
			blur : {
				delegateType : "focusout"
			},
			beforeunload : {
				setup : function (A, B, _) {
					if (n.isWindow(this)) {
						this.onbeforeunload = _;
					}
				},
				teardown : function (A, _) {
					if (this.onbeforeunload === _) {
						this.onbeforeunload = null;
					}
				}
			}
		},
		simulate : function (B, _, C, A) {
			var D = n.extend(new n.Event(), C, {
					type : B,
					isSimulated : true,
					originalEvent : {}

				});
			if (A) {
				n.event.trigger(D, null, _);
			} else {
				n.event.dispatch.call(_, D);
			}
			if (D.isDefaultPrevented()) {
				C.preventDefault();
			}
		}
	};
	n.event.handle = n.event.dispatch;
	n.removeEvent = y0.removeEventListener ? function (A, B, _) {
		if (A.removeEventListener) {
			A.removeEventListener(B, _, false);
		}
	}
	 : function (A, B, _) {
		if (A.detachEvent) {
			A.detachEvent("on" + B, _);
		}
	};
	n.Event = function (A, _) {
		if (!(this instanceof n.Event)) {
			return new n.Event(A, _);
		}
		if (A && A.type) {
			this.originalEvent = A;
			this.type = A.type;
			this.isDefaultPrevented = (A.defaultPrevented || A.returnValue === false || A.getPreventDefault && A.getPreventDefault()) ? O0 : H;
		} else {
			this.type = A;
		}
		if (_) {
			n.extend(this, _);
		}
		this.timeStamp = A && A.timeStamp || n.now();
		this[n.expando] = true;
	};
	function H() {
		return false;
	}
	function O0() {
		return true;
	}
	n.Event.prototype = {
		preventDefault : function () {
			this.isDefaultPrevented = O0;
			var _ = this.originalEvent;
			if (!_) {
				return;
			}
			if (_.preventDefault) {
				_.preventDefault();
			} else {
				_.returnValue = false;
			}
		},
		stopPropagation : function () {
			this.isPropagationStopped = O0;
			var _ = this.originalEvent;
			if (!_) {
				return;
			}
			if (_.stopPropagation) {
				_.stopPropagation();
			}
			_.cancelBubble = true;
		},
		stopImmediatePropagation : function () {
			this.isImmediatePropagationStopped = O0;
			this.stopPropagation();
		},
		isDefaultPrevented : H,
		isPropagationStopped : H,
		isImmediatePropagationStopped : H
	};
	n.each({
		mouseenter : "mouseover",
		mouseleave : "mouseout"
	}, function (A, _) {
		n.event.special[A] = {
			delegateType : _,
			bindType : _,
			handle : function (D) {
				var B = this,
				F = D.relatedTarget,
				C = D.handleObj,
				A = C.selector,
				E;
				if (!F || (F !== B && !n.contains(B, F))) {
					D.type = C.origType;
					E = C.handler.apply(this, arguments);
					D.type = _;
				}
				return E;
			}
		};
	});
	if (!n.support.submitBubbles) {
		n.event.special.submit = {
			setup : function () {
				if (n.nodeName(this, "form")) {
					return false;
				}
				n.event.add(this, "click._submit keypress._submit", function (B) {
					var _ = B.target,
					A = n.nodeName(_, "input") || n.nodeName(_, "button") ? _.form : C1;
					if (A && !A._submit_attached) {
						n.event.add(A, "submit._submit", function (_) {
							_._submit_bubble = true;
						});
						A._submit_attached = true;
					}
				});
			},
			postDispatch : function (_) {
				if (_._submit_bubble) {
					delete _._submit_bubble;
					if (this.parentNode && !_.isTrigger) {
						n.event.simulate("submit", this.parentNode, _, true);
					}
				}
			},
			teardown : function () {
				if (n.nodeName(this, "form")) {
					return false;
				}
				n.event.remove(this, "._submit");
			}
		};
	}
	if (!n.support.changeBubbles) {
		n.event.special.change = {
			setup : function () {
				if (s0.test(this.nodeName)) {
					if (this.type === "checkbox" || this.type === "radio") {
						n.event.add(this, "propertychange._change", function (_) {
							if (_.originalEvent.propertyName === "checked") {
								this._just_changed = true;
							}
						});
						n.event.add(this, "click._change", function (_) {
							if (this._just_changed && !_.isTrigger) {
								this._just_changed = false;
								n.event.simulate("change", this, _, true);
							}
						});
					}
					return false;
				}
				n.event.add(this, "beforeactivate._change", function (A) {
					var _ = A.target;
					if (s0.test(_.nodeName) && !_._change_attached) {
						n.event.add(_, "change._change", function (_) {
							if (this.parentNode && !_.isSimulated && !_.isTrigger) {
								n.event.simulate("change", this.parentNode, _, true);
							}
						});
						_._change_attached = true;
					}
				});
			},
			handle : function (A) {
				var _ = A.target;
				if (this !== _ || A.isSimulated || A.isTrigger || (_.type !== "radio" && _.type !== "checkbox")) {
					return A.handleObj.handler.apply(this, arguments);
				}
			},
			teardown : function () {
				n.event.remove(this, "._change");
				return s0.test(this.nodeName);
			}
		};
	}
	if (!n.support.focusinBubbles) {
		n.each({
			focus : "focusin",
			blur : "focusout"
		}, function (C, A) {
			var B = 0,
			_ = function (_) {
				n.event.simulate(A, _.target, n.event.fix(_), true);
			};
			n.event.special[A] = {
				setup : function () {
					if (B++ === 0) {
						y0.addEventListener(C, _, true);
					}
				},
				teardown : function () {
					if (--B === 0) {
						y0.removeEventListener(C, _, true);
					}
				}
			};
		});
	}
	n.fn.extend({
		on : function (A, B, D, F, _) {
			var E,
			C;
			if (typeof A === "object") {
				if (typeof B !== "string") {
					D = D || B;
					B = C1;
				}
				for (C in A) {
					this.on(C, B, D, A[C], _);
				}
				return this;
			}
			if (D == null && F == null) {
				F = B;
				D = B = C1;
			} else {
				if (F == null) {
					if (typeof B === "string") {
						F = D;
						D = C1;
					} else {
						F = D;
						D = B;
						B = C1;
					}
				}
			}
			if (F === false) {
				F = H;
			} else {
				if (!F) {
					return this;
				}
			}
			if (_ === 1) {
				E = F;
				F = function (_) {
					n().off(_);
					return E.apply(this, arguments);
				};
				F.guid = E.guid || (E.guid = n.guid++);
			}
			return this.each(function () {
				n.event.add(this, A, F, D, B);
			});
		},
		one : function (_, A, B, C) {
			return this.on(_, A, B, C, 1);
		},
		off : function (_, A, D) {
			if (_ && _.preventDefault && _.handleObj) {
				var C = _.handleObj;
				n(_.delegateTarget).off(C.namespace ? C.origType + "." + C.namespace : C.origType, C.selector, C.handler);
				return this;
			}
			if (typeof _ === "object") {
				for (var B in _) {
					this.off(B, A, _[B]);
				}
				return this;
			}
			if (A === false || typeof A === "function") {
				D = A;
				A = C1;
			}
			if (D === false) {
				D = H;
			}
			return this.each(function () {
				n.event.remove(this, _, D, A);
			});
		},
		bind : function (_, A, B) {
			return this.on(_, null, A, B);
		},
		unbind : function (_, A) {
			return this.off(_, null, A);
		},
		live : function (_, A, B) {
			n(this.context).on(_, this.selector, A, B);
			return this;
		},
		die : function (_, A) {
			n(this.context).off(_, this.selector || "**", A);
			return this;
		},
		delegate : function (A, _, B, C) {
			return this.on(_, A, B, C);
		},
		undelegate : function (A, _, B) {
			return arguments.length == 1 ? this.off(A, "**") : this.off(_, A, B);
		},
		trigger : function (_, A) {
			return this.each(function () {
				n.event.trigger(_, A, this);
			});
		},
		triggerHandler : function (_, A) {
			if (this[0]) {
				return n.event.trigger(_, A, this[0], true);
			}
		},
		toggle : function (D) {
			var B = arguments,
			C = D.guid || n.guid++,
			_ = 0,
			A = function (C) {
				var A = (n._data(this, "lastToggle" + D.guid) || 0) % _;
				n._data(this, "lastToggle" + D.guid, A + 1);
				C.preventDefault();
				return B[A].apply(this, arguments) || false;
			};
			A.guid = C;
			while (_ < B.length) {
				B[_++].guid = C;
			}
			return this.click(A);
		},
		hover : function (_, A) {
			return this.mouseenter(_).mouseleave(A || _);
		}
	});
	n.each(("blur focus focusin focusout load resize scroll unload click dblclick " + "mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave " + "change select submit keydown keypress keyup error contextmenu").split(" "), function (_, A) {
		n.fn[A] = function (_, B) {
			if (B == null) {
				B = _;
				_ = null;
			}
			return arguments.length > 0 ? this.on(A, null, _, B) : this.trigger(A);
		};
		if (n.attrFn) {
			n.attrFn[A] = true;
		}
		if (L.test(A)) {
			n.event.fixHooks[A] = n.event.keyHooks;
		}
		if (E1.test(A)) {
			n.event.fixHooks[A] = n.event.mouseHooks;
		}
	});
	(function () {
		var L = /((?:\((?:\([^()]+\)|[^()]+)+\)|\[(?:\[[^\[\]]*\]|['"][^'"]*['"]|[^\[\]'"]+)+\]|\\.|[^ >+~,(\[\\]+)+|[>+~])(\s*,\s*)?((?:.|\r|\n)*)/g,
		O = "sizcache" + (Math.random() + "").replace(".", ""),
		G = 0,
		F = Object.prototype.toString,
		U = false,
		C = true,
		S = /\\/g,
		H = /\r\n/g,
		E = /\W/;
		[0, 0].sort(function () {
			C = false;
			return 0;
		});
		var _ = function (C, Q, H, U) {
			H = H || [];
			Q = Q || y0;
			var W = Q;
			if (Q.nodeType !== 1 && Q.nodeType !== 9) {
				return [];
			}
			if (!C || typeof C !== "string") {
				return H;
			}
			var O,
			P,
			V,
			E,
			S,
			D,
			T,
			J,
			G = true,
			I = _.isXML(Q),
			A = [],
			B = C;
			do {
				L.exec("");
				O = L.exec(B);
				if (O) {
					B = O[3];
					A.push(O[1]);
					if (O[2]) {
						E = O[3];
						break;
					}
				}
			} while (O);
			if (A.length > 1 && M.exec(C)) {
				if (A.length === 2 && R.relative[A[0]]) {
					P = N(A[0] + A[1], Q, U);
				} else {
					P = R.relative[A[0]] ? [Q] : _(A.shift(), Q);
					while (A.length) {
						C = A.shift();
						if (R.relative[C]) {
							C += A.shift();
						}
						P = N(C, P, U);
					}
				}
			} else {
				if (!U && A.length > 1 && Q.nodeType === 9 && !I && R.match.ID.test(A[0]) && !R.match.ID.test(A[A.length - 1])) {
					S = _.find(A.shift(), Q, I);
					Q = S.expr ? _.filter(S.expr, S.set)[0] : S.set[0];
				}
				if (Q) {
					S = U ? {
						expr : A.pop(),
						set : K(U)
					}
					 : _.find(A.pop(), A.length === 1 && (A[0] === "~" || A[0] === "+") && Q.parentNode ? Q.parentNode : Q, I);
					P = S.expr ? _.filter(S.expr, S.set) : S.set;
					if (A.length > 0) {
						V = K(P);
					} else {
						G = false;
					}
					while (A.length) {
						D = A.pop();
						T = D;
						if (!R.relative[D]) {
							D = "";
						} else {
							T = A.pop();
						}
						if (T == null) {
							T = Q;
						}
						R.relative[D](V, T, I);
					}
				} else {
					V = A = [];
				}
			}
			if (!V) {
				V = P;
			}
			if (!V) {
				_.error(D || C);
			}
			if (F.call(V) === "[object Array]") {
				if (!G) {
					H.push.apply(H, V);
				} else {
					if (Q && Q.nodeType === 1) {
						for (J = 0; V[J] != null; J++) {
							if (V[J] && (V[J] === true || V[J].nodeType === 1 && _.contains(Q, V[J]))) {
								H.push(P[J]);
							}
						}
					} else {
						for (J = 0; V[J] != null; J++) {
							if (V[J] && V[J].nodeType === 1) {
								H.push(P[J]);
							}
						}
					}
				}
			} else {
				K(V, H);
			}
			if (E) {
				_(E, W, H, U);
				_.uniqueSort(H);
			}
			return H;
		};
		_.uniqueSort = function (A) {
			if (J) {
				U = C;
				A.sort(J);
				if (U) {
					for (var _ = 1; _ < A.length; _++) {
						if (A[_] === A[_ - 1]) {
							A.splice(_--, 1);
						}
					}
				}
			}
			return A;
		};
		_.matches = function (A, B) {
			return _(A, null, null, B);
		};
		_.matchesSelector = function (B, A) {
			return _(A, null, null, [B]).length > 0;
		};
		_.find = function (C, G, H) {
			var F,
			B,
			A,
			_,
			D,
			E;
			if (!C) {
				return [];
			}
			for (B = 0, A = R.order.length; B < A; B++) {
				D = R.order[B];
				if ((_ = R.leftMatch[D].exec(C))) {
					E = _[1];
					_.splice(1, 1);
					if (E.substr(E.length - 1) !== "\\") {
						_[1] = (_[1] || "").replace(S, "");
						F = R.find[D](_, G, H);
						if (F != null) {
							C = C.replace(R.match[D], "");
							break;
						}
					}
				}
			}
			if (!F) {
				F = typeof G.getElementsByTagName !== "undefined" ? G.getElementsByTagName("*") : [];
			}
			return {
				set : F,
				expr : C
			};
		};
		_.filter = function (K, M, F, D) {
			var G,
			Q,
			B,
			E,
			A,
			H,
			N,
			I,
			C,
			J = K,
			O = [],
			P = M,
			L = M && M[0] && _.isXML(M[0]);
			while (K && M.length) {
				for (B in R.filter) {
					if ((G = R.leftMatch[B].exec(K)) != null && G[2]) {
						H = R.filter[B];
						N = G[1];
						Q = false;
						G.splice(1, 1);
						if (N.substr(N.length - 1) === "\\") {
							continue;
						}
						if (P === O) {
							O = [];
						}
						if (R.preFilter[B]) {
							G = R.preFilter[B](G, P, F, O, D, L);
							if (!G) {
								Q = E = true;
							} else {
								if (G === true) {
									continue;
								}
							}
						}
						if (G) {
							for (I = 0; (A = P[I]) != null; I++) {
								if (A) {
									E = H(A, G, I, P);
									C = D^E;
									if (F && E != null) {
										if (C) {
											Q = true;
										} else {
											P[I] = false;
										}
									} else {
										if (C) {
											O.push(A);
											Q = true;
										}
									}
								}
							}
						}
						if (E !== C1) {
							if (!F) {
								P = O;
							}
							K = K.replace(R.match[B], "");
							if (!Q) {
								return [];
							}
							break;
						}
					}
				}
				if (K === J) {
					if (Q == null) {
						_.error(K);
					} else {
						break;
					}
				}
				J = K;
			}
			return P;
		};
		_.error = function (_) {
			throw new Error("Syntax error, unrecognized expression: " + _);
		};
		var A = _.getText = function (B) {
			var _,
			C,
			D = B.nodeType,
			E = "";
			if (D) {
				if (D === 1 || D === 9 || D === 11) {
					if (typeof B.textContent === "string") {
						return B.textContent;
					} else {
						if (typeof B.innerText === "string") {
							return B.innerText.replace(H, "");
						} else {
							for (B = B.firstChild; B; B = B.nextSibling) {
								E += A(B);
							}
						}
					}
				} else {
					if (D === 3 || D === 4) {
						return B.nodeValue;
					}
				}
			} else {
				for (_ = 0; (C = B[_]); _++) {
					if (C.nodeType !== 8) {
						E += A(C);
					}
				}
			}
			return E;
		},
		R = _.selectors = {
			order : ["ID", "NAME", "TAG"],
			match : {
				ID : /#((?:[\w\u00c0-\uFFFF\-]|\\.)+)/,
				CLASS : /\.((?:[\w\u00c0-\uFFFF\-]|\\.)+)/,
				NAME : /\[name=['"]*((?:[\w\u00c0-\uFFFF\-]|\\.)+)['"]*\]/,
				ATTR : /\[\s*((?:[\w\u00c0-\uFFFF\-]|\\.)+)\s*(?:(\S?=)\s*(?:(['"])(.*?)\3|(#?(?:[\w\u00c0-\uFFFF\-]|\\.)*)|)|)\s*\]/,
				TAG : /^((?:[\w\u00c0-\uFFFF\*\-]|\\.)+)/,
				CHILD : /:(only|nth|last|first)-child(?:\(\s*(even|odd|(?:[+\-]?\d+|(?:[+\-]?\d*)?n\s*(?:[+\-]\s*\d+)?))\s*\))?/,
				POS : /:(nth|eq|gt|lt|first|last|even|odd)(?:\((\d*)\))?(?=[^\-]|$)/,
				PSEUDO : /:((?:[\w\u00c0-\uFFFF\-]|\\.)+)(?:\((['"]?)((?:\([^\)]+\)|[^\(\)]*)+)\2\))?/
			},
			leftMatch : {},
			attrMap : {
				"class" : "className",
				"for" : "htmlFor"
			},
			attrHandle : {
				href : function (_) {
					return _.getAttribute("href");
				},
				type : function (_) {
					return _.getAttribute("type");
				}
			},
			relative : {
				"+" : function (I, G) {
					var C = typeof G === "string",
					F = C && !E.test(G),
					A = C && !F;
					if (F) {
						G = G.toLowerCase();
					}
					for (var B = 0, H = I.length, D; B < H; B++) {
						if ((D = I[B])) {
							while ((D = D.previousSibling) && D.nodeType !== 1) {}

							I[B] = A || D && D.nodeName.toLowerCase() === G ? D || false : D === G;
						}
					}
					if (A) {
						_.filter(G, I, true);
					}
				},
				">" : function (G, D) {
					var C,
					B = typeof D === "string",
					A = 0,
					F = G.length;
					if (B && !E.test(D)) {
						D = D.toLowerCase();
						for (; A < F; A++) {
							C = G[A];
							if (C) {
								var H = C.parentNode;
								G[A] = H.nodeName.toLowerCase() === D ? H : false;
							}
						}
					} else {
						for (; A < F; A++) {
							C = G[A];
							if (C) {
								G[A] = B ? C.parentNode : C.parentNode === D;
							}
						}
						if (B) {
							_.filter(D, G, true);
						}
					}
				},
				"" : function (D, B, F) {
					var A,
					C = G++,
					_ = I;
					if (typeof B === "string" && !E.test(B)) {
						B = B.toLowerCase();
						A = B;
						_ = P;
					}
					_("parentNode", B, C, D, A, F);
				},
				"~" : function (D, B, F) {
					var A,
					C = G++,
					_ = I;
					if (typeof B === "string" && !E.test(B)) {
						B = B.toLowerCase();
						A = B;
						_ = P;
					}
					_("previousSibling", B, C, D, A, F);
				}
			},
			find : {
				ID : function (_, B, C) {
					if (typeof B.getElementById !== "undefined" && !C) {
						var A = B.getElementById(_[1]);
						return A && A.parentNode ? [A] : [];
					}
				},
				NAME : function (_, B) {
					if (typeof B.getElementsByName !== "undefined") {
						var C = [],
						E = B.getElementsByName(_[1]);
						for (var A = 0, D = E.length; A < D; A++) {
							if (E[A].getAttribute("name") === _[1]) {
								C.push(E[A]);
							}
						}
						return C.length === 0 ? null : C;
					}
				},
				TAG : function (_, A) {
					if (typeof A.getElementsByTagName !== "undefined") {
						return A.getElementsByTagName(_[1]);
					}
				}
			},
			preFilter : {
				CLASS : function (A, F, _, D, E, G) {
					A = " " + A[1].replace(S, "") + " ";
					if (G) {
						return A;
					}
					for (var B = 0, C; (C = F[B]) != null; B++) {
						if (C) {
							if (E^(C.className && (" " + C.className + " ").replace(/[\t\n\r]/g, " ").indexOf(A) >= 0)) {
								if (!_) {
									D.push(C);
								}
							} else {
								if (_) {
									F[B] = false;
								}
							}
						}
					}
					return false;
				},
				ID : function (_) {
					return _[1].replace(S, "");
				},
				TAG : function (_, A) {
					return _[1].replace(S, "").toLowerCase();
				},
				CHILD : function (A) {
					if (A[1] === "nth") {
						if (!A[2]) {
							_.error(A[0]);
						}
						A[2] = A[2].replace(/^\+|\s*/g, "");
						var B = /(-?)(\d*)(?:n([+\-]?\d*))?/.exec(A[2] === "even" && "2n" || A[2] === "odd" && "2n+1" || !/\D/.test(A[2]) && "0n+" + A[2] || A[2]);
						A[2] = (B[1] + (B[2] || 1)) - 0;
						A[3] = B[3] - 0;
					} else {
						if (A[2]) {
							_.error(A[0]);
						}
					}
					A[0] = G++;
					return A;
				},
				ATTR : function (A, D, _, B, C, F) {
					var E = A[1] = A[1].replace(S, "");
					if (!F && R.attrMap[E]) {
						A[1] = R.attrMap[E];
					}
					A[4] = (A[4] || A[5] || "").replace(S, "");
					if (A[2] === "~=") {
						A[4] = " " + A[4] + " ";
					}
					return A;
				},
				PSEUDO : function (B, F, A, C, E) {
					if (B[1] === "not") {
						if ((L.exec(B[3]) || "").length > 1 || /^\w/.test(B[3])) {
							B[3] = _(B[3], null, null, F);
						} else {
							var D = _.filter(B[3], F, A, true^E);
							if (!A) {
								C.push.apply(C, D);
							}
							return false;
						}
					} else {
						if (R.match.POS.test(B[0]) || R.match.CHILD.test(B[0])) {
							return true;
						}
					}
					return B;
				},
				POS : function (_) {
					_.unshift(true);
					return _;
				}
			},
			filters : {
				enabled : function (_) {
					return _.disabled === false && _.type !== "hidden";
				},
				disabled : function (_) {
					return _.disabled === true;
				},
				checked : function (_) {
					return _.checked === true;
				},
				selected : function (_) {
					if (_.parentNode) {
						_.parentNode.selectedIndex;
					}
					return _.selected === true;
				},
				parent : function (_) {
					return !!_.firstChild;
				},
				empty : function (_) {
					return !_.firstChild;
				},
				has : function (C, B, A) {
					return !!_(A[3], C).length;
				},
				header : function (_) {
					return (/h\d/i).test(_.nodeName);
				},
				text : function (_) {
					var B = _.getAttribute("type"),
					A = _.type;
					return _.nodeName.toLowerCase() === "input" && "text" === A && (B === A || B === null);
				},
				radio : function (_) {
					return _.nodeName.toLowerCase() === "input" && "radio" === _.type;
				},
				checkbox : function (_) {
					return _.nodeName.toLowerCase() === "input" && "checkbox" === _.type;
				},
				file : function (_) {
					return _.nodeName.toLowerCase() === "input" && "file" === _.type;
				},
				password : function (_) {
					return _.nodeName.toLowerCase() === "input" && "password" === _.type;
				},
				submit : function (_) {
					var A = _.nodeName.toLowerCase();
					return (A === "input" || A === "button") && "submit" === _.type;
				},
				image : function (_) {
					return _.nodeName.toLowerCase() === "input" && "image" === _.type;
				},
				reset : function (_) {
					var A = _.nodeName.toLowerCase();
					return (A === "input" || A === "button") && "reset" === _.type;
				},
				button : function (_) {
					var A = _.nodeName.toLowerCase();
					return A === "input" && "button" === _.type || A === "button";
				},
				input : function (_) {
					return (/input|select|textarea|button/i).test(_.nodeName);
				},
				focus : function (_) {
					return _ === _.ownerDocument.activeElement;
				}
			},
			setFilters : {
				first : function (A, _) {
					return _ === 0;
				},
				last : function (B, A, _, C) {
					return A === C.length - 1;
				},
				even : function (A, _) {
					return _ % 2 === 0;
				},
				odd : function (A, _) {
					return _ % 2 === 1;
				},
				lt : function (B, A, _) {
					return A < _[3] - 0;
				},
				gt : function (B, A, _) {
					return A > _[3] - 0;
				},
				nth : function (B, A, _) {
					return _[3] - 0 === A;
				},
				eq : function (B, A, _) {
					return _[3] - 0 === A;
				}
			},
			filter : {
				PSEUDO : function (E, C, D, J) {
					var I = C[1],
					B = R.filters[I];
					if (B) {
						return B(E, D, C, J);
					} else {
						if (I === "contains") {
							return (E.textContent || E.innerText || A([E]) || "").indexOf(C[3]) >= 0;
						} else {
							if (I === "not") {
								var G = C[3];
								for (var F = 0, H = G.length; F < H; F++) {
									if (G[F] === E) {
										return false;
									}
								}
								return true;
							} else {
								_.error(I);
							}
						}
					}
				},
				CHILD : function (C, A) {
					var F,
					B,
					G,
					J,
					_,
					I,
					H,
					D = A[1],
					E = C;
					switch (D) {
					case "only":
					case "first":
						while ((E = E.previousSibling)) {
							if (E.nodeType === 1) {
								return false;
							}
						}
						if (D === "first") {
							return true;
						}
						E = C;
					case "last":
						while ((E = E.nextSibling)) {
							if (E.nodeType === 1) {
								return false;
							}
						}
						return true;
					case "nth":
						F = A[2];
						B = A[3];
						if (F === 1 && B === 0) {
							return true;
						}
						G = A[0];
						J = C.parentNode;
						if (J && (J[O] !== G || !C.nodeIndex)) {
							I = 0;
							for (E = J.firstChild; E; E = E.nextSibling) {
								if (E.nodeType === 1) {
									E.nodeIndex = ++I;
								}
							}
							J[O] = G;
						}
						H = C.nodeIndex - B;
						if (F === 0) {
							return H === 0;
						} else {
							return (H % F === 0 && H / F >= 0);
						}
					}
				},
				ID : function (A, _) {
					return A.nodeType === 1 && A.getAttribute("id") === _;
				},
				TAG : function (A, _) {
					return (_ === "*" && A.nodeType === 1) || !!A.nodeName && A.nodeName.toLowerCase() === _;
				},
				CLASS : function (A, _) {
					return (" " + (A.className || A.getAttribute("class")) + " ").indexOf(_) > -1;
				},
				ATTR : function (C, A) {
					var G = A[1],
					E = _.attr ? _.attr(C, G) : R.attrHandle[G] ? R.attrHandle[G](C) : C[G] != null ? C[G] : C.getAttribute(G),
					B = E + "",
					D = A[2],
					F = A[4];
					return E == null ? D === "!=" : !D && _.attr ? E != null : D === "=" ? B === F : D === "*=" ? B.indexOf(F) >= 0 : D === "~=" ? (" " + B + " ").indexOf(F) >= 0 : !F ? B && E !== false : D === "!=" ? B !== F : D === "^=" ? B.indexOf(F) === 0 : D === "$=" ? B.substr(B.length - F.length) === F : D === "|=" ? B === F || B.substr(0, F.length + 1) === F + "-" : false;
				},
				POS : function (C, A, B, E) {
					var D = A[2],
					_ = R.setFilters[D];
					if (_) {
						return _(C, B, A, E);
					}
				}
			}
		},
		M = R.match.POS,
		Q = function (A, _) {
			return "\\" + (_ - 0 + 1);
		};
		for (var D in R.match) {
			R.match[D] = new RegExp(R.match[D].source + (/(?![^\[]*\])(?![^\(]*\))/.source));
			R.leftMatch[D] = new RegExp(/(^(?:.|\r|\n)*?)/.source + R.match[D].source.replace(/\\(\d+)/g, Q));
		}
		R.match.globalPOS = M;
		var K = function (A, _) {
			A = Array.prototype.slice.call(A, 0);
			if (_) {
				_.push.apply(_, A);
				return _;
			}
			return A;
		};
		try {
			Array.prototype.slice.call(y0.documentElement.childNodes, 0)[0].nodeType;
		} catch (T) {
			K = function (D, C) {
				var _ = 0,
				A = C || [];
				if (F.call(D) === "[object Array]") {
					Array.prototype.push.apply(A, D);
				} else {
					if (typeof D.length === "number") {
						for (var B = D.length; _ < B; _++) {
							A.push(D[_]);
						}
					} else {
						for (; D[_]; _++) {
							A.push(D[_]);
						}
					}
				}
				return A;
			};
		}
		var J,
		B;
		if (y0.documentElement.compareDocumentPosition) {
			J = function (_, A) {
				if (_ === A) {
					U = true;
					return 0;
				}
				if (!_.compareDocumentPosition || !A.compareDocumentPosition) {
					return _.compareDocumentPosition ? -1 : 1;
				}
				return _.compareDocumentPosition(A) & 4 ? -1 : 1;
			};
		} else {
			J = function (E, J) {
				if (E === J) {
					U = true;
					return 0;
				} else {
					if (E.sourceIndex && J.sourceIndex) {
						return E.sourceIndex - J.sourceIndex;
					}
				}
				var A,
				G,
				D = [],
				I = [],
				C = E.parentNode,
				H = J.parentNode,
				F = C;
				if (C === H) {
					return B(E, J);
				} else {
					if (!C) {
						return -1;
					} else {
						if (!H) {
							return 1;
						}
					}
				}
				while (F) {
					D.unshift(F);
					F = F.parentNode;
				}
				F = H;
				while (F) {
					I.unshift(F);
					F = F.parentNode;
				}
				A = D.length;
				G = I.length;
				for (var _ = 0; _ < A && _ < G; _++) {
					if (D[_] !== I[_]) {
						return B(D[_], I[_]);
					}
				}
				return _ === A ? B(E, I[_], -1) : B(D[_], J, 1);
			};
			B = function (_, C, B) {
				if (_ === C) {
					return B;
				}
				var A = _.nextSibling;
				while (A) {
					if (A === C) {
						return -1;
					}
					A = A.nextSibling;
				}
				return 1;
			};
		}
		(function () {
			var _ = y0.createElement("div"),
			B = "script" + (new Date()).getTime(),
			A = y0.documentElement;
			_.innerHTML = "<a name='" + B + "'/>";
			A.insertBefore(_, A.firstChild);
			if (y0.getElementById(B)) {
				R.find.ID = function (_, B, C) {
					if (typeof B.getElementById !== "undefined" && !C) {
						var A = B.getElementById(_[1]);
						return A ? A.id === _[1] || typeof A.getAttributeNode !== "undefined" && A.getAttributeNode("id").nodeValue === _[1] ? [A] : C1 : [];
					}
				};
				R.filter.ID = function (A, _) {
					var B = typeof A.getAttributeNode !== "undefined" && A.getAttributeNode("id");
					return A.nodeType === 1 && B && B.nodeValue === _;
				};
			}
			A.removeChild(_);
			A = _ = null;
		})();
		(function () {
			var _ = y0.createElement("div");
			_.appendChild(y0.createComment(""));
			if (_.getElementsByTagName("*").length > 0) {
				R.find.TAG = function (_, C) {
					var D = C.getElementsByTagName(_[1]);
					if (_[1] === "*") {
						var B = [];
						for (var A = 0; D[A]; A++) {
							if (D[A].nodeType === 1) {
								B.push(D[A]);
							}
						}
						D = B;
					}
					return D;
				};
			}
			_.innerHTML = "<a href='#'></a>";
			if (_.firstChild && typeof _.firstChild.getAttribute !== "undefined" && _.firstChild.getAttribute("href") !== "#") {
				R.attrHandle.href = function (_) {
					return _.getAttribute("href", 2);
				};
			}
			_ = null;
		})();
		if (y0.querySelectorAll) {
			(function () {
				var A = _,
				B = y0.createElement("div"),
				D = "__sizzle__";
				B.innerHTML = "<p class='TEST'></p>";
				if (B.querySelectorAll && B.querySelectorAll(".TEST").length === 0) {
					return;
				}
				_ = function (N, O, I, P) {
					O = O || y0;
					if (!P && !_.isXML(O)) {
						var L = /^(\w+$)|^\.([\w\-]+$)|^#([\w\-]+$)/.exec(N);
						if (L && (O.nodeType === 1 || O.nodeType === 9)) {
							if (L[1]) {
								return K(O.getElementsByTagName(N), I);
							} else {
								if (L[2] && R.find.CLASS && O.getElementsByClassName) {
									return K(O.getElementsByClassName(L[2]), I);
								}
							}
						}
						if (O.nodeType === 9) {
							if (N === "body" && O.body) {
								return K([O.body], I);
							} else {
								if (L && L[3]) {
									var C = O.getElementById(L[3]);
									if (C && C.parentNode) {
										if (C.id === L[3]) {
											return K([C], I);
										}
									} else {
										return K([], I);
									}
								}
							}
							try {
								return K(O.querySelectorAll(N), I);
							} catch (F) {}

						} else {
							if (O.nodeType === 1 && O.nodeName.toLowerCase() !== "object") {
								var E = O,
								M = O.getAttribute("id"),
								G = M || D,
								H = O.parentNode,
								J = /^\s*[+~]/.test(N);
								if (!M) {
									O.setAttribute("id", G);
								} else {
									G = G.replace(/'/g, "\\$&");
								}
								if (J && H) {
									O = O.parentNode;
								}
								try {
									if (!J || H) {
										return K(O.querySelectorAll("[id='" + G + "'] " + N), I);
									}
								} catch (B) {}

								finally {
									if (!M) {
										E.removeAttribute("id");
									}
								}
							}
						}
					}
					return A(N, O, I, P);
				};
				for (var C in A) {
					_[C] = A[C];
				}
				B = null;
			})();
		}
		(function () {
			var E = y0.documentElement,
			D = E.matchesSelector || E.mozMatchesSelector || E.webkitMatchesSelector || E.msMatchesSelector;
			if (D) {
				var B = !D.call(y0.createElement("div"), "div"),
				C = false;
				try {
					D.call(y0.documentElement, "[test!='']:sizzle");
				} catch (A) {
					C = true;
				}
				_.matchesSelector = function (E, A) {
					A = A.replace(/\=\s*([^'"\]]*)\s*\]/g, "='$1']");
					if (!_.isXML(E)) {
						try {
							if (C || !R.match.PSEUDO.test(A) && !/!=/.test(A)) {
								var F = D.call(E, A);
								if (F || !B || E.document && E.document.nodeType !== 11) {
									return F;
								}
							}
						} catch (G) {}

					}
					return _(A, null, null, [E]).length > 0;
				};
			}
		})();
		(function () {
			var _ = y0.createElement("div");
			_.innerHTML = "<div class='test e'></div><div class='test'></div>";
			if (!_.getElementsByClassName || _.getElementsByClassName("e").length === 0) {
				return;
			}
			_.lastChild.className = "e";
			if (_.getElementsByClassName("e").length === 1) {
				return;
			}
			R.order.splice(1, 0, "CLASS");
			R.find.CLASS = function (_, A, B) {
				if (typeof A.getElementsByClassName !== "undefined" && !B) {
					return A.getElementsByClassName(_[1]);
				}
			};
			_ = null;
		})();
		function P(H, D, E, G, B, I) {
			for (var A = 0, F = G.length; A < F; A++) {
				var C = G[A];
				if (C) {
					var _ = false;
					C = C[H];
					while (C) {
						if (C[O] === E) {
							_ = G[C.sizset];
							break;
						}
						if (C.nodeType === 1 && !I) {
							C[O] = E;
							C.sizset = A;
						}
						if (C.nodeName.toLowerCase() === D) {
							_ = C;
							break;
						}
						C = C[H];
					}
					G[A] = _;
				}
			}
		}
		function I(I, E, F, H, C, J) {
			for (var B = 0, G = H.length; B < G; B++) {
				var D = H[B];
				if (D) {
					var A = false;
					D = D[I];
					while (D) {
						if (D[O] === F) {
							A = H[D.sizset];
							break;
						}
						if (D.nodeType === 1) {
							if (!J) {
								D[O] = F;
								D.sizset = B;
							}
							if (typeof E !== "string") {
								if (D === E) {
									A = true;
									break;
								}
							} else {
								if (_.filter(E, [D]).length > 0) {
									A = D;
									break;
								}
							}
						}
						D = D[I];
					}
					H[B] = A;
				}
			}
		}
		if (y0.documentElement.contains) {
			_.contains = function (_, A) {
				return _ !== A && (_.contains ? _.contains(A) : true);
			};
		} else {
			if (y0.documentElement.compareDocumentPosition) {
				_.contains = function (_, A) {
					return !!(_.compareDocumentPosition(A) & 16);
				};
			} else {
				_.contains = function () {
					return false;
				};
			}
		}
		_.isXML = function (_) {
			var A = (_ ? _.ownerDocument || _ : 0).documentElement;
			return A ? A.nodeName !== "HTML" : false;
		};
		var N = function (E, F, H) {
			var A,
			D = [],
			C = "",
			G = F.nodeType ? [F] : F;
			while ((A = R.match.PSEUDO.exec(E))) {
				C += A[0];
				E = E.replace(R.match.PSEUDO, "");
			}
			E = R.relative[E] ? E + "*" : E;
			for (var B = 0, I = G.length; B < I; B++) {
				_(E, G[B], D, H);
			}
			return _.filter(C, D);
		};
		_.attr = n.attr;
		_.selectors.attrMap = {};
		n.find = _;
		n.expr = _.selectors;
		n.expr[":"] = n.expr.filters;
		n.unique = _.uniqueSort;
		n.text = _.getText;
		n.isXMLDoc = _.isXML;
		n.contains = _.contains;
	})();
	var I0 = /Until$/,
	I = /^(?:parents|prevUntil|prevAll)/,
	x0 = /,/,
	A = /^.[^:#\[\.,]*$/,
	W0 = Array.prototype.slice,
	e = n.expr.match.globalPOS,
	a0 = {
		children : true,
		contents : true,
		next : true,
		prev : true
	};
	n.fn.extend({
		find : function (A) {
			var C = this,
			_,
			G;
			if (typeof A !== "string") {
				return n(A).filter(function () {
					for (_ = 0, G = C.length; _ < G; _++) {
						if (n.contains(C[_], this)) {
							return true;
						}
					}
				});
			}
			var E = this.pushStack("", "find", A),
			D,
			F,
			B;
			for (_ = 0, G = this.length; _ < G; _++) {
				D = E.length;
				n.find(A, this[_], E);
				if (_ > 0) {
					for (F = D; F < E.length; F++) {
						for (B = 0; B < D; B++) {
							if (E[B] === E[F]) {
								E.splice(F--, 1);
								break;
							}
						}
					}
				}
			}
			return E;
		},
		has : function (_) {
			var A = n(_);
			return this.filter(function () {
				for (var _ = 0, B = A.length; _ < B; _++) {
					if (n.contains(this, A[_])) {
						return true;
					}
				}
			});
		},
		not : function (_) {
			return this.pushStack(k(this, _, false), "not", _);
		},
		filter : function (_) {
			return this.pushStack(k(this, _, true), "filter", _);
		},
		is : function (_) {
			return !!_ && (typeof _ === "string" ? e.test(_) ? n(_, this.context).index(this[0]) >= 0 : n.filter(_, this).length > 0 : this.filter(_).length > 0);
		},
		closest : function (G, D) {
			var E = [],
			A,
			F,
			C = this[0];
			if (n.isArray(G)) {
				var _ = 1;
				while (C && C.ownerDocument && C !== D) {
					for (A = 0; A < G.length; A++) {
						if (n(C).is(G[A])) {
							E.push({
								selector : G[A],
								elem : C,
								level : _
							});
						}
					}
					C = C.parentNode;
					_++;
				}
				return E;
			}
			var B = e.test(G) || typeof G !== "string" ? n(G, D || this.context) : 0;
			for (A = 0, F = this.length; A < F; A++) {
				C = this[A];
				while (C) {
					if (B ? B.index(C) > -1 : n.find.matchesSelector(C, G)) {
						E.push(C);
						break;
					} else {
						C = C.parentNode;
						if (!C || !C.ownerDocument || C === D || C.nodeType === 11) {
							break;
						}
					}
				}
			}
			E = E.length > 1 ? n.unique(E) : E;
			return this.pushStack(E, "closest", G);
		},
		index : function (_) {
			if (!_) {
				return (this[0] && this[0].parentNode) ? this.prevAll().length : -1;
			}
			if (typeof _ === "string") {
				return n.inArray(this[0], n(_));
			}
			return n.inArray(_.jquery ? _[0] : _, this);
		},
		add : function (_, B) {
			var A = typeof _ === "string" ? n(_, B) : n.makeArray(_ && _.nodeType ? [_] : _),
			C = n.merge(this.get(), A);
			return this.pushStack(y(A[0]) || y(C[0]) ? C : n.unique(C));
		},
		andSelf : function () {
			return this.add(this.prevObject);
		}
	});
	function y(_) {
		return !_ || !_.parentNode || _.parentNode.nodeType === 11;
	}
	n.each({
		parent : function (_) {
			var A = _.parentNode;
			return A && A.nodeType !== 11 ? A : null;
		},
		parents : function (_) {
			return n.dir(_, "parentNode");
		},
		parentsUntil : function (B, A, _) {
			return n.dir(B, "parentNode", _);
		},
		next : function (_) {
			return n.nth(_, 2, "nextSibling");
		},
		prev : function (_) {
			return n.nth(_, 2, "previousSibling");
		},
		nextAll : function (_) {
			return n.dir(_, "nextSibling");
		},
		prevAll : function (_) {
			return n.dir(_, "previousSibling");
		},
		nextUntil : function (B, A, _) {
			return n.dir(B, "nextSibling", _);
		},
		prevUntil : function (B, A, _) {
			return n.dir(B, "previousSibling", _);
		},
		siblings : function (_) {
			return n.sibling((_.parentNode || {}).firstChild, _);
		},
		children : function (_) {
			return n.sibling(_.firstChild);
		},
		contents : function (_) {
			return n.nodeName(_, "iframe") ? _.contentDocument || _.contentWindow.document : n.makeArray(_.childNodes);
		}
	}, function (_, A) {
		n.fn[_] = function (B, C) {
			var D = n.map(this, A, B);
			if (!I0.test(_)) {
				C = B;
			}
			if (C && typeof C === "string") {
				D = n.filter(C, D);
			}
			D = this.length > 1 && !a0[_] ? n.unique(D) : D;
			if ((this.length > 1 || x0.test(C)) && I.test(_)) {
				D = D.reverse();
			}
			return this.pushStack(D, _, W0.call(arguments).join(","));
		};
	});
	n.extend({
		filter : function (_, A, B) {
			if (B) {
				_ = ":not(" + _ + ")";
			}
			return A.length === 1 ? n.find.matchesSelector(A[0], _) ? [A[0]] : [] : n.find.matches(_, A);
		},
		dir : function (A, D, _) {
			var B = [],
			C = A[D];
			while (C && C.nodeType !== 9 && (_ === C1 || C.nodeType !== 1 || !n(C).is(_))) {
				if (C.nodeType === 1) {
					B.push(C);
				}
				C = C[D];
			}
			return B;
		},
		nth : function (C, B, D, A) {
			B = B || 1;
			var _ = 0;
			for (; C; C = C[D]) {
				if (C.nodeType === 1 && ++_ === B) {
					break;
				}
			}
			return C;
		},
		sibling : function (B, _) {
			var A = [];
			for (; B; B = B.nextSibling) {
				if (B.nodeType === 1 && B !== _) {
					A.push(B);
				}
			}
			return A;
		}
	});
	function k(D, B, _) {
		B = B || 0;
		if (n.isFunction(B)) {
			return n.grep(D, function (C, A) {
				var D = !!B.call(C, A, C);
				return D === _;
			});
		} else {
			if (B.nodeType) {
				return n.grep(D, function (C, A) {
					return (C === B) === _;
				});
			} else {
				if (typeof B === "string") {
					var C = n.grep(D, function (_) {
							return _.nodeType === 1;
						});
					if (A.test(B)) {
						return n.filter(B, C, !_);
					} else {
						B = n.filter(B, C);
					}
				}
			}
		}
		return n.grep(D, function (C, A) {
			return (n.inArray(C, B) >= 0) === _;
		});
	}
	function p(A) {
		var _ = F1.split("|"),
		B = A.createDocumentFragment();
		if (B.createElement) {
			while (_.length) {
				B.createElement(_.pop());
			}
		}
		return B;
	}
	var F1 = "abbr|article|aside|audio|bdi|canvas|data|datalist|details|figcaption|figure|footer|" + "header|hgroup|mark|meter|nav|output|progress|section|summary|time|video",
	h0 = / jQuery\d+="(?:\d+|null)"/g,
	E = /^\s+/,
	u0 = /<(?!area|br|col|embed|hr|img|input|link|meta|param)(([\w:]+)[^>]*)\/>/ig,
	N1 = /<([\w:]+)/,
	f = /<tbody/i,
	_1 = /<|&#?\w+;/,
	S0 = /<(?:script|style)/i,
	A0 = /<(?:script|object|embed|option|style)/i,
	p0 = new RegExp("<(?:" + F1 + ")[\\s/>]", "i"),
	u = /checked\s*(?:[^=]|=\s*.checked.)/i,
	F = /\/(java|ecma)script/i,
	D1 = /^\s*<!(?:\[CDATA\[|\-\-)/,
	l = {
		option : [1, "<select multiple='multiple'>", "</select>"],
		legend : [1, "<fieldset>", "</fieldset>"],
		thead : [1, "<table>", "</table>"],
		tr : [2, "<table><tbody>", "</tbody></table>"],
		td : [3, "<table><tbody><tr>", "</tr></tbody></table>"],
		col : [2, "<table><tbody></tbody><colgroup>", "</colgroup></table>"],
		area : [1, "<map>", "</map>"],
		_default : [0, "", ""]
	},
	c0 = p(y0);
	l.optgroup = l.option;
	l.tbody = l.tfoot = l.colgroup = l.caption = l.thead;
	l.th = l.td;
	if (!n.support.htmlSerialize) {
		l._default = [1, "div<div>", "</div>"];
	}
	n.fn.extend({
		text : function (_) {
			return n.access(this, function (_) {
				return _ === C1 ? n.text(this) : this.empty().append((this[0] && this[0].ownerDocument || y0).createTextNode(_));
			}, null, _, arguments.length);
		},
		wrapAll : function (_) {
			if (n.isFunction(_)) {
				return this.each(function (A) {
					n(this).wrapAll(_.call(this, A));
				});
			}
			if (this[0]) {
				var A = n(_, this[0].ownerDocument).eq(0).clone(true);
				if (this[0].parentNode) {
					A.insertBefore(this[0]);
				}
				A.map(function () {
					var _ = this;
					while (_.firstChild && _.firstChild.nodeType === 1) {
						_ = _.firstChild;
					}
					return _;
				}).append(this);
			}
			return this;
		},
		wrapInner : function (_) {
			if (n.isFunction(_)) {
				return this.each(function (A) {
					n(this).wrapInner(_.call(this, A));
				});
			}
			return this.each(function () {
				var B = n(this),
				A = B.contents();
				if (A.length) {
					A.wrapAll(_);
				} else {
					B.append(_);
				}
			});
		},
		wrap : function (_) {
			var A = n.isFunction(_);
			return this.each(function (B) {
				n(this).wrapAll(A ? _.call(this, B) : _);
			});
		},
		unwrap : function () {
			return this.parent().each(function () {
				if (!n.nodeName(this, "body")) {
					n(this).replaceWith(this.childNodes);
				}
			}).end();
		},
		append : function () {
			return this.domManip(arguments, true, function (_) {
				if (this.nodeType === 1) {
					this.appendChild(_);
				}
			});
		},
		prepend : function () {
			return this.domManip(arguments, true, function (_) {
				if (this.nodeType === 1) {
					this.insertBefore(_, this.firstChild);
				}
			});
		},
		before : function () {
			if (this[0] && this[0].parentNode) {
				return this.domManip(arguments, false, function (_) {
					this.parentNode.insertBefore(_, this);
				});
			} else {
				if (arguments.length) {
					var _ = n.clean(arguments);
					_.push.apply(_, this.toArray());
					return this.pushStack(_, "before", arguments);
				}
			}
		},
		after : function () {
			if (this[0] && this[0].parentNode) {
				return this.domManip(arguments, false, function (_) {
					this.parentNode.insertBefore(_, this.nextSibling);
				});
			} else {
				if (arguments.length) {
					var _ = this.pushStack(this, "after", arguments);
					_.push.apply(_, n.clean(arguments));
					return _;
				}
			}
		},
		remove : function (C, B) {
			for (var _ = 0, A; (A = this[_]) != null; _++) {
				if (!C || n.filter(C, [A]).length) {
					if (!B && A.nodeType === 1) {
						n.cleanData(A.getElementsByTagName("*"));
						n.cleanData([A]);
					}
					if (A.parentNode) {
						A.parentNode.removeChild(A);
					}
				}
			}
			return this;
		},
		empty : function () {
			for (var _ = 0, A; (A = this[_]) != null; _++) {
				if (A.nodeType === 1) {
					n.cleanData(A.getElementsByTagName("*"));
				}
				while (A.firstChild) {
					A.removeChild(A.firstChild);
				}
			}
			return this;
		},
		clone : function (A, _) {
			A = A == null ? false : A;
			_ = _ == null ? A : _;
			return this.map(function () {
				return n.clone(this, A, _);
			});
		},
		html : function (_) {
			return n.access(this, function (B) {
				var A = this[0] || {},
				_ = 0,
				C = this.length;
				if (B === C1) {
					return A.nodeType === 1 ? A.innerHTML.replace(h0, "") : null;
				}
				if (typeof B === "string" && !S0.test(B) && (n.support.leadingWhitespace || !E.test(B)) && !l[(N1.exec(B) || ["", ""])[1].toLowerCase()]) {
					B = B.replace(u0, "<$1></$2>");
					try {
						for (; _ < C; _++) {
							A = this[_] || {};
							if (A.nodeType === 1) {
								n.cleanData(A.getElementsByTagName("*"));
								A.innerHTML = B;
							}
						}
						A = 0;
					} catch (D) {}

				}
				if (A) {
					this.empty().append(B);
				}
			}, null, _, arguments.length);
		},
		replaceWith : function (_) {
			if (this[0] && this[0].parentNode) {
				if (n.isFunction(_)) {
					return this.each(function (A) {
						var C = n(this),
						B = C.html();
						C.replaceWith(_.call(this, A, B));
					});
				}
				if (typeof _ !== "string") {
					_ = n(_).detach();
				}
				return this.each(function () {
					var A = this.nextSibling,
					B = this.parentNode;
					n(this).remove();
					if (A) {
						n(A).before(_);
					} else {
						n(B).append(_);
					}
				});
			} else {
				return this.length ? this.pushStack(n(n.isFunction(_) ? _() : _), "replaceWith", _) : this;
			}
		},
		detach : function (_) {
			return this.remove(_, true);
		},
		domManip : function (C, _, I) {
			var G,
			D,
			J,
			K,
			A = C[0],
			B = [];
			if (!n.support.checkClone && arguments.length === 3 && typeof A === "string" && u.test(A)) {
				return this.each(function () {
					n(this).domManip(C, _, I, true);
				});
			}
			if (n.isFunction(A)) {
				return this.each(function (B) {
					var D = n(this);
					C[0] = A.call(this, B, _ ? D.html() : C1);
					D.domManip(C, _, I);
				});
			}
			if (this[0]) {
				K = A && A.parentNode;
				if (n.support.parentNode && K && K.nodeType === 11 && K.childNodes.length === this.length) {
					G = {
						fragment : K
					};
				} else {
					G = n.buildFragment(C, this, B);
				}
				J = G.fragment;
				if (J.childNodes.length === 1) {
					D = J = J.firstChild;
				} else {
					D = J.firstChild;
				}
				if (D) {
					_ = _ && n.nodeName(D, "tr");
					for (var H = 0, F = this.length, E = F - 1; H < F; H++) {
						I.call(_ ? V1(this[H], D) : this[H], G.cacheable || (F > 1 && H < E) ? n.clone(J, true, true) : J);
					}
				}
				if (B.length) {
					n.each(B, function (_, A) {
						if (A.src) {
							n.ajax({
								type : "GET",
								global : false,
								url : A.src,
								async : false,
								dataType : "script"
							});
						} else {
							n.globalEval((A.text || A.textContent || A.innerHTML || "").replace(D1, "/*$0*/"));
						}
						if (A.parentNode) {
							A.parentNode.removeChild(A);
						}
					});
				}
			}
			return this;
		}
	});
	function V1(_, A) {
		return n.nodeName(_, "table") ? (_.getElementsByTagName("tbody")[0] || _.appendChild(_.ownerDocument.createElement("tbody"))) : _;
	}
	function R1(E, G) {
		if (G.nodeType !== 1 || !n.hasData(E)) {
			return;
		}
		var D,
		A,
		F,
		B = n._data(E),
		C = n._data(G, B),
		_ = B.events;
		if (_) {
			delete C.handle;
			C.events = {};
			for (D in _) {
				for (A = 0, F = _[D].length; A < F; A++) {
					n.event.add(G, D, _[D][A]);
				}
			}
		}
		if (C.data) {
			C.data = n.extend({}, C.data);
		}
	}
	function _(A, B) {
		var _;
		if (B.nodeType !== 1) {
			return;
		}
		if (B.clearAttributes) {
			B.clearAttributes();
		}
		if (B.mergeAttributes) {
			B.mergeAttributes(A);
		}
		_ = B.nodeName.toLowerCase();
		if (_ === "object") {
			B.outerHTML = A.outerHTML;
		} else {
			if (_ === "input" && (A.type === "checkbox" || A.type === "radio")) {
				if (A.checked) {
					B.defaultChecked = B.checked = A.checked;
				}
				if (B.value !== A.value) {
					B.value = A.value;
				}
			} else {
				if (_ === "option") {
					B.selected = A.defaultSelected;
				} else {
					if (_ === "input" || _ === "textarea") {
						B.defaultValue = A.defaultValue;
					} else {
						if (_ === "script" && B.text !== A.text) {
							B.text = A.text;
						}
					}
				}
			}
		}
		B.removeAttribute(n.expando);
		B.removeAttribute("_submit_attached");
		B.removeAttribute("_change_attached");
	}
	n.buildFragment = function (D, G, B) {
		var C,
		F,
		A,
		_,
		E = D[0];
		if (G && G[0]) {
			_ = G[0].ownerDocument || G[0];
		}
		if (!_.createDocumentFragment) {
			_ = y0;
		}
		if (D.length === 1 && typeof E === "string" && E.length < 512 && _ === y0 && E.charAt(0) === "<" && !A0.test(E) && (n.support.checkClone || !u.test(E)) && (n.support.html5Clone || !p0.test(E))) {
			F = true;
			A = n.fragments[E];
			if (A && A !== 1) {
				C = A;
			}
		}
		if (!C) {
			C = _.createDocumentFragment();
			n.clean(D, _, C, B);
		}
		if (F) {
			n.fragments[E] = A ? C : 1;
		}
		return {
			fragment : C,
			cacheable : F
		};
	};
	n.fragments = {};
	n.each({
		appendTo : "append",
		prependTo : "prepend",
		insertBefore : "before",
		insertAfter : "after",
		replaceAll : "replaceWith"
	}, function (A, _) {
		n.fn[A] = function (C) {
			var F = [],
			E = n(C),
			H = this.length === 1 && this[0].parentNode;
			if (H && H.nodeType === 11 && H.childNodes.length === 1 && E.length === 1) {
				E[_](this[0]);
				return this;
			} else {
				for (var B = 0, G = E.length; B < G; B++) {
					var D = (B > 0 ? this.clone(true) : this).get();
					n(E[B])[_](D);
					F = F.concat(D);
				}
				return this.pushStack(F, A, E.selector);
			}
		};
	});
	function L0(_) {
		if (typeof _.getElementsByTagName !== "undefined") {
			return _.getElementsByTagName("*");
		} else {
			if (typeof _.querySelectorAll !== "undefined") {
				return _.querySelectorAll("*");
			} else {
				return [];
			}
		}
	}
	function e0(_) {
		if (_.type === "checkbox" || _.type === "radio") {
			_.defaultChecked = _.checked;
		}
	}
	function q(_) {
		var A = (_.nodeName || "").toLowerCase();
		if (A === "input") {
			e0(_);
		} else {
			if (A !== "script" && typeof _.getElementsByTagName !== "undefined") {
				n.grep(_.getElementsByTagName("input"), e0);
			}
		}
	}
	function T0(_) {
		var A = y0.createElement("div");
		c0.appendChild(A);
		A.innerHTML = _.outerHTML;
		return A.firstChild;
	}
	n.extend({
		clone : function (B, G, F) {
			var E,
			C,
			A,
			D = n.support.html5Clone || n.isXMLDoc(B) || !p0.test("<" + B.nodeName + ">") ? B.cloneNode(true) : T0(B);
			if ((!n.support.noCloneEvent || !n.support.noCloneChecked) && (B.nodeType === 1 || B.nodeType === 11) && !n.isXMLDoc(B)) {
				_(B, D);
				E = L0(B);
				C = L0(D);
				for (A = 0; E[A]; ++A) {
					if (C[A]) {
						_(E[A], C[A]);
					}
				}
			}
			if (G) {
				R1(B, D);
				if (F) {
					E = L0(B);
					C = L0(D);
					for (A = 0; E[A]; ++A) {
						R1(E[A], C[A]);
					}
				}
			}
			E = C = null;
			return D;
		},
		clean : function (P, Q, O, C) {
			var G,
			D,
			S,
			T = [];
			Q = Q || y0;
			if (typeof Q.createElement === "undefined") {
				Q = Q.ownerDocument || Q[0] && Q[0].ownerDocument || y0;
			}
			for (var M = 0, B; (B = P[M]) != null; M++) {
				if (typeof B === "number") {
					B += "";
				}
				if (!B) {
					continue;
				}
				if (typeof B === "string") {
					if (!_1.test(B)) {
						B = Q.createTextNode(B);
					} else {
						B = B.replace(u0, "<$1></$2>");
						var R = (N1.exec(B) || ["", ""])[1].toLowerCase(),
						K = l[R] || l._default,
						H = K[0],
						N = Q.createElement("div"),
						I = c0.childNodes,
						J;
						if (Q === y0) {
							c0.appendChild(N);
						} else {
							p(Q).appendChild(N);
						}
						N.innerHTML = K[1] + B + K[2];
						while (H--) {
							N = N.lastChild;
						}
						if (!n.support.tbody) {
							var L = f.test(B),
							U = R === "table" && !L ? N.firstChild && N.firstChild.childNodes : K[1] === "<table>" && !L ? N.childNodes : [];
							for (S = U.length - 1; S >= 0; --S) {
								if (n.nodeName(U[S], "tbody") && !U[S].childNodes.length) {
									U[S].parentNode.removeChild(U[S]);
								}
							}
						}
						if (!n.support.leadingWhitespace && E.test(B)) {
							N.insertBefore(Q.createTextNode(E.exec(B)[0]), N.firstChild);
						}
						B = N.childNodes;
						if (N) {
							N.parentNode.removeChild(N);
							if (I.length > 0) {
								J = I[I.length - 1];
								if (J && J.parentNode) {
									J.parentNode.removeChild(J);
								}
							}
						}
					}
				}
				var _;
				if (!n.support.appendChecked) {
					if (B[0] && typeof(_ = B.length) === "number") {
						for (S = 0; S < _; S++) {
							q(B[S]);
						}
					} else {
						q(B);
					}
				}
				if (B.nodeType) {
					T.push(B);
				} else {
					T = n.merge(T, B);
				}
			}
			if (O) {
				G = function (_) {
					return !_.type || F.test(_.type);
				};
				for (M = 0; T[M]; M++) {
					D = T[M];
					if (C && n.nodeName(D, "script") && (!D.type || F.test(D.type))) {
						C.push(D.parentNode ? D.parentNode.removeChild(D) : D);
					} else {
						if (D.nodeType === 1) {
							var A = n.grep(D.getElementsByTagName("script"), G);
							T.splice.apply(T, [M + 1, 0].concat(A));
						}
						O.appendChild(D);
					}
				}
			}
			return T;
		},
		cleanData : function (F) {
			var E,
			H,
			_ = n.cache,
			D = n.event.special,
			G = n.support.deleteExpando;
			for (var A = 0, B; (B = F[A]) != null; A++) {
				if (B.nodeName && n.noData[B.nodeName.toLowerCase()]) {
					continue;
				}
				H = B[n.expando];
				if (H) {
					E = _[H];
					if (E && E.events) {
						for (var C in E.events) {
							if (D[C]) {
								n.event.remove(B, C);
							} else {
								n.removeEvent(B, C, E.handle);
							}
						}
						if (E.handle) {
							E.handle.elem = null;
						}
					}
					if (G) {
						delete B[n.expando];
					} else {
						if (B.removeAttribute) {
							B.removeAttribute(n.expando);
						}
					}
					delete _[H];
				}
			}
		}
	});
	var i = /alpha\([^)]*\)/i,
	T1 = /opacity=([^)]*)/,
	o = /([A-Z]|^ms)/g,
	x = /^[\-+]?(?:\d*\.)?\d+$/i,
	J0 = /^-?(?:\d*\.)?\d+(?!px)[^\d\s]+$/i,
	C0 = /^([\-+])=([\-+.\de]+)/,
	f0 = /^margin/,
	Q = {
		position : "absolute",
		visibility : "hidden",
		display : "block"
	},
	O = ["Top", "Right", "Bottom", "Left"],
	K0,
	P0,
	B;
	n.fn.css = function (A, _) {
		return n.access(this, function (A, B, _) {
			return _ !== C1 ? n.style(A, B, _) : n.css(A, B);
		}, A, _, arguments.length > 1);
	};
	n.extend({
		cssHooks : {
			opacity : {
				get : function (_, B) {
					if (B) {
						var A = K0(_, "opacity");
						return A === "" ? "1" : A;
					} else {
						return _.style.opacity;
					}
				}
			}
		},
		cssNumber : {
			fillOpacity : true,
			fontWeight : true,
			lineHeight : true,
			opacity : true,
			orphans : true,
			widows : true,
			zIndex : true,
			zoom : true
		},
		cssProps : {
			"float" : n.support.cssFloat ? "cssFloat" : "styleFloat"
		},
		style : function (A, H, _, D) {
			if (!A || A.nodeType === 3 || A.nodeType === 8 || !A.style) {
				return;
			}
			var G,
			B,
			F = n.camelCase(H),
			E = A.style,
			C = n.cssHooks[F];
			H = n.cssProps[F] || F;
			if (_ !== C1) {
				B = typeof _;
				if (B === "string" && (G = C0.exec(_))) {
					_ = ( + (G[1] + 1) * +G[2]) + parseFloat(n.css(A, H));
					B = "number";
				}
				if (_ == null || B === "number" && isNaN(_)) {
					return;
				}
				if (B === "number" && !n.cssNumber[F]) {
					_ += "px";
				}
				if (!C || !("set" in C) || (_ = C.set(A, _)) !== C1) {
					try {
						E[H] = _;
					} catch (I) {}

				}
			} else {
				if (C && "get" in C && (G = C.get(A, false, D)) !== C1) {
					return G;
				}
				return E[H];
			}
		},
		css : function (_, D, B) {
			var C,
			A;
			D = n.camelCase(D);
			A = n.cssHooks[D];
			D = n.cssProps[D] || D;
			if (D === "cssFloat") {
				D = "float";
			}
			if (A && "get" in A && (C = A.get(_, true, B)) !== C1) {
				return C;
			} else {
				if (K0) {
					return K0(_, D);
				}
			}
		},
		swap : function (A, E, _) {
			var B = {},
			C,
			D;
			for (D in E) {
				B[D] = A.style[D];
				A.style[D] = E[D];
			}
			C = _.call(A);
			for (D in E) {
				A.style[D] = B[D];
			}
			return C;
		}
	});
	n.curCSS = n.css;
	if (y0.defaultView && y0.defaultView.getComputedStyle) {
		P0 = function (_, E) {
			var D,
			F,
			B,
			A,
			C = _.style;
			E = E.replace(o, "-$1").toLowerCase();
			if ((F = _.ownerDocument.defaultView) && (B = F.getComputedStyle(_, null))) {
				D = B.getPropertyValue(E);
				if (D === "" && !n.contains(_.ownerDocument.documentElement, _)) {
					D = n.style(_, E);
				}
			}
			if (!n.support.pixelMargin && B && f0.test(E) && J0.test(D)) {
				A = C.width;
				C.width = D;
				D = B.width;
				C.width = A;
			}
			return D;
		};
	}
	if (y0.documentElement.currentStyle) {
		B = function (B, F) {
			var C,
			A,
			_,
			E = B.currentStyle && B.currentStyle[F],
			D = B.style;
			if (E == null && D && (_ = D[F])) {
				E = _;
			}
			if (J0.test(E)) {
				C = D.left;
				A = B.runtimeStyle && B.runtimeStyle.left;
				if (A) {
					B.runtimeStyle.left = B.currentStyle.left;
				}
				D.left = F === "fontSize" ? "1em" : E;
				E = D.pixelLeft + "px";
				D.left = C;
				if (A) {
					B.runtimeStyle.left = A;
				}
			}
			return E === "" ? "auto" : E;
		};
	}
	K0 = P0 || B;
	function Q1(C, E, D) {
		var B = E === "width" ? C.offsetWidth : C.offsetHeight,
		A = E === "width" ? 1 : 0,
		_ = 4;
		if (B > 0) {
			if (D !== "border") {
				for (; A < _; A += 2) {
					if (!D) {
						B -= parseFloat(n.css(C, "padding" + O[A])) || 0;
					}
					if (D === "margin") {
						B += parseFloat(n.css(C, D + O[A])) || 0;
					} else {
						B -= parseFloat(n.css(C, "border" + O[A] + "Width")) || 0;
					}
				}
			}
			return B + "px";
		}
		B = K0(C, E);
		if (B < 0 || B == null) {
			B = C.style[E];
		}
		if (J0.test(B)) {
			return B;
		}
		B = parseFloat(B) || 0;
		if (D) {
			for (; A < _; A += 2) {
				B += parseFloat(n.css(C, "padding" + O[A])) || 0;
				if (D !== "padding") {
					B += parseFloat(n.css(C, "border" + O[A] + "Width")) || 0;
				}
				if (D === "margin") {
					B += parseFloat(n.css(C, D + O[A])) || 0;
				}
			}
		}
		return B + "px";
	}
	n.each(["height", "width"], function (_, A) {
		n.cssHooks[A] = {
			get : function (_, C, B) {
				if (C) {
					if (_.offsetWidth !== 0) {
						return Q1(_, A, B);
					} else {
						return n.swap(_, Q, function () {
							return Q1(_, A, B);
						});
					}
				}
			},
			set : function (A, _) {
				return x.test(_) ? _ + "px" : _;
			}
		};
	});
	if (!n.support.opacity) {
		n.cssHooks.opacity = {
			get : function (_, A) {
				return T1.test((A && _.currentStyle ? _.currentStyle.filter : _.style.filter) || "") ? (parseFloat(RegExp.$1) / 100) + "" : A ? "1" : "";
			},
			set : function (B, A) {
				var E = B.style,
				C = B.currentStyle,
				D = n.isNumeric(A) ? "alpha(opacity=" + A * 100 + ")" : "",
				_ = C && C.filter || E.filter || "";
				E.zoom = 1;
				if (A >= 1 && n.trim(_.replace(i, "")) === "") {
					E.removeAttribute("filter");
					if (C && !C.filter) {
						return;
					}
				}
				E.filter = i.test(_) ? _.replace(i, D) : _ + " " + D;
			}
		};
	}
	n(function () {
		if (!n.support.reliableMarginRight) {
			n.cssHooks.marginRight = {
				get : function (_, A) {
					return n.swap(_, {
						display : "inline-block"
					}, function () {
						if (A) {
							return K0(_, "margin-right");
						} else {
							return _.style.marginRight;
						}
					});
				}
			};
		}
	});
	if (n.expr && n.expr.filters) {
		n.expr.filters.hidden = function (_) {
			var A = _.offsetWidth,
			B = _.offsetHeight;
			return (A === 0 && B === 0) || (!n.support.reliableHiddenOffsets && ((_.style && _.style.display) || n.css(_, "display")) === "none");
		};
		n.expr.filters.visible = function (_) {
			return !n.expr.filters.hidden(_);
		};
	}
	n.each({
		margin : "",
		padding : "",
		border : "Width"
	}, function (A, _) {
		n.cssHooks[A + _] = {
			expand : function (D) {
				var C,
				B = typeof D === "string" ? D.split(" ") : [D],
				E = {};
				for (C = 0; C < 4; C++) {
					E[A + O[C] + _] = B[C] || B[C - 2] || B[0];
				}
				return E;
			}
		};
	});
	var c = /%20/g,
	G0 = /\[\]$/,
	L1 = /\r?\n/g,
	B1 = /#.*$/,
	Y1 = /^(.*?):[ \t]*([^\r\n]*)\r?$/mg,
	U1 = /^(?:color|date|datetime|datetime-local|email|hidden|month|number|password|range|search|tel|text|time|url|week)$/i,
	a = /^(?:about|app|app\-storage|.+\-extension|file|res|widget):$/,
	t0 = /^(?:GET|HEAD)$/,
	U0 = /^\/\//,
	R = /\?/,
	H1 = /<script\b[^<]*(?:(?!<\/script>)<[^<]*)*<\/script>/gi,
	m0 = /^(?:select|textarea)/i,
	r0 = /\s+/,
	Z1 = /([?&])_=[^&]*/,
	V = /^([\w\+\.\-]+:)(?:\/\/([^\/?#:]*)(?::(\d+))?)?/,
	z = n.fn.load,
	S = {},
	C = {},
	Z0,
	o0,
	j0 = ["*/"] + ["*"];
	try {
		Z0 = P.href;
	} catch (g0) {
		Z0 = y0.createElement("a");
		Z0.href = "";
		Z0 = Z0.href;
	}
	o0 = V.exec(Z0.toLowerCase()) || [];
	function b(_) {
		return function (F, E) {
			if (typeof F !== "string") {
				E = F;
				F = "*";
			}
			if (n.isFunction(E)) {
				var H = F.toLowerCase().split(r0),
				B = 0,
				D = H.length,
				C,
				A,
				G;
				for (; B < D; B++) {
					C = H[B];
					G = /^\+/.test(C);
					if (G) {
						C = C.substr(1) || "*";
					}
					A = _[C] = _[C] || [];
					A[G ? "unshift" : "push"](E);
				}
			}
		};
	}
	function Y(H, J, C, A, E, D) {
		E = E || J.dataTypes[0];
		D = D || {};
		D[E] = true;
		var _ = H[E],
		B = 0,
		F = _ ? _.length : 0,
		I = (H === S),
		G;
		for (; B < F && (I || !G); B++) {
			G = _[B](J, C, A);
			if (typeof G === "string") {
				if (!I || D[G]) {
					G = C1;
				} else {
					J.dataTypes.unshift(G);
					G = Y(H, J, C, A, G, D);
				}
			}
		}
		if ((I || !G) && !D["*"]) {
			G = Y(H, J, C, A, "*", D);
		}
		return G;
	}
	function M(A, C) {
		var _,
		D,
		B = n.ajaxSettings.flatOptions || {};
		for (_ in C) {
			if (C[_] !== C1) {
				(B[_] ? A : (D || (D = {})))[_] = C[_];
			}
		}
		if (D) {
			n.extend(true, A, D);
		}
	}
	n.fn.extend({
		load : function (E, D, _) {
			if (typeof E !== "string" && z) {
				return z.apply(this, arguments);
			} else {
				if (!this.length) {
					return this;
				}
			}
			var F = E.indexOf(" ");
			if (F >= 0) {
				var A = E.slice(F, E.length);
				E = E.slice(0, F);
			}
			var C = "GET";
			if (D) {
				if (n.isFunction(D)) {
					_ = D;
					D = C1;
				} else {
					if (typeof D === "object") {
						D = n.param(D, n.ajaxSettings.traditional);
						C = "POST";
					}
				}
			}
			var B = this;
			n.ajax({
				url : E,
				type : C,
				dataType : "html",
				data : D,
				complete : function (C, D, E) {
					E = C.responseText;
					if (C.isResolved()) {
						C.done(function (_) {
							E = _;
						});
						B.html(A ? n("<div>").append(E.replace(H1, "")).find(A) : E);
					}
					if (_) {
						B.each(_, [E, D, C]);
					}
				}
			});
			return this;
		},
		serialize : function () {
			return n.param(this.serializeArray());
		},
		serializeArray : function () {
			return this.map(function () {
				return this.elements ? n.makeArray(this.elements) : this;
			}).filter(function () {
				return this.name && !this.disabled && (this.checked || m0.test(this.nodeName) || U1.test(this.type));
			}).map(function (A, B) {
				var _ = n(this).val();
				return _ == null ? null : n.isArray(_) ? n.map(_, function (A, _) {
					return {
						name : B.name,
						value : A.replace(L1, "\r\n")
					};
				}) : {
					name : B.name,
					value : _.replace(L1, "\r\n")
				};
			}).get();
		}
	});
	n.each("ajaxStart ajaxStop ajaxComplete ajaxError ajaxSuccess ajaxSend".split(" "), function (A, _) {
		n.fn[_] = function (A) {
			return this.on(_, A);
		};
	});
	n.each(["get", "post"], function (_, A) {
		n[A] = function (D, C, _, B) {
			if (n.isFunction(C)) {
				B = B || _;
				_ = C;
				C = C1;
			}
			return n.ajax({
				type : A,
				url : D,
				data : C,
				success : _,
				dataType : B
			});
		};
	});
	n.extend({
		getScript : function (A, _) {
			return n.get(A, C1, _, "script");
		},
		getJSON : function (B, A, _) {
			return n.get(B, A, _, "json");
		},
		ajaxSetup : function (A, _) {
			if (_) {
				M(A, n.ajaxSettings);
			} else {
				_ = A;
				A = n.ajaxSettings;
			}
			M(A, _);
			return A;
		},
		ajaxSettings : {
			url : Z0,
			isLocal : a.test(o0[1]),
			global : true,
			type : "GET",
			contentType : "application/x-www-form-urlencoded; charset=UTF-8",
			processData : true,
			async : true,
			accepts : {
				xml : "application/xml, text/xml",
				html : "text/html",
				text : "text/plain",
				json : "application/json, text/javascript",
				"*" : j0
			},
			contents : {
				xml : /xml/,
				html : /html/,
				json : /json/
			},
			responseFields : {
				xml : "responseXML",
				text : "responseText"
			},
			converters : {
				"* text" : N0.String,
				"text html" : true,
				"text json" : n.parseJSON,
				"text xml" : n.parseXML
			},
			flatOptions : {
				context : true,
				url : true
			}
		},
		ajaxPrefilter : b(S),
		ajaxTransport : b(C),
		ajax : function (L, Z) {
			if (typeof L === "object") {
				Z = L;
				L = C1;
			}
			Z = Z || {};
			var M = n.ajaxSetup({}, Z),
			T = M.context || M,
			A = T !== M && (T.nodeType || T instanceof n) ? n(T) : n.event,
			O = n.Deferred(),
			D = n.Callbacks("once memory"),
			N = M.statusCode || {},
			K,
			Q = {},
			B = {},
			X,
			H,
			G,
			F,
			_,
			a = 0,
			W,
			I,
			P = {
				readyState : 0,
				setRequestHeader : function (C, A) {
					if (!a) {
						var _ = C.toLowerCase();
						C = B[_] = B[_] || C;
						Q[C] = A;
					}
					return this;
				},
				getAllResponseHeaders : function () {
					return a === 2 ? X : null;
				},
				getResponseHeader : function (A) {
					var _;
					if (a === 2) {
						if (!H) {
							H = {};
							while ((_ = Y1.exec(X))) {
								H[_[1].toLowerCase()] = _[2];
							}
						}
						_ = H[A.toLowerCase()];
					}
					return _ === C1 ? null : _;
				},
				overrideMimeType : function (_) {
					if (!a) {
						M.mimeType = _;
					}
					return this;
				},
				abort : function (_) {
					_ = _ || "abort";
					if (G) {
						G.abort(_);
					}
					E(0, _);
					return this;
				}
			};
			function E(E, B, _, C) {
				if (a === 2) {
					return;
				}
				a = 2;
				if (F) {
					clearTimeout(F);
				}
				G = C1;
				X = C || "";
				P.readyState = E > 0 ? 4 : 0;
				var J,
				R,
				H,
				L = B,
				I = _ ? R0(M, P, _) : C1,
				Q,
				S;
				if (E >= 200 && E < 300 || E === 304) {
					if (M.ifModified) {
						if ((Q = P.getResponseHeader("Last-Modified"))) {
							n.lastModified[K] = Q;
						}
						if ((S = P.getResponseHeader("Etag"))) {
							n.etag[K] = S;
						}
					}
					if (E === 304) {
						L = "notmodified";
						J = true;
					} else {
						try {
							R = U(M, I);
							L = "success";
							J = true;
						} catch (V) {
							L = "parsererror";
							H = V;
						}
					}
				} else {
					H = L;
					if (!L || E) {
						L = "error";
						if (E < 0) {
							E = 0;
						}
					}
				}
				P.status = E;
				P.statusText = "" + (B || L);
				if (J) {
					O.resolveWith(T, [R, L, P]);
				} else {
					O.rejectWith(T, [P, L, H]);
				}
				P.statusCode(N);
				N = C1;
				if (W) {
					A.trigger("ajax" + (J ? "Success" : "Error"), [P, M, J ? R : H]);
				}
				D.fireWith(T, [P, L]);
				if (W) {
					A.trigger("ajaxComplete", [P, M]);
					if (!(--n.active)) {
						n.event.trigger("ajaxStop");
					}
				}
			}
			O.promise(P);
			P.success = P.done;
			P.error = P.fail;
			P.complete = D.add;
			P.statusCode = function (A) {
				if (A) {
					var _;
					if (a < 2) {
						for (_ in A) {
							N[_] = [N[_], A[_]];
						}
					} else {
						_ = A[P.status];
						P.then(_, _);
					}
				}
				return this;
			};
			M.url = ((L || M.url) + "").replace(B1, "").replace(U0, o0[1] + "//");
			M.dataTypes = n.trim(M.dataType || "*").toLowerCase().split(r0);
			if (M.crossDomain == null) {
				_ = V.exec(M.url.toLowerCase());
				M.crossDomain = !!(_ && (_[1] != o0[1] || _[2] != o0[2] || (_[3] || (_[1] === "http:" ? 80 : 443)) != (o0[3] || (o0[1] === "http:" ? 80 : 443))));
			}
			if (M.data && M.processData && typeof M.data !== "string") {
				M.data = n.param(M.data, M.traditional);
			}
			Y(S, M, Z, P);
			if (a === 2) {
				return false;
			}
			W = M.global;
			M.type = M.type.toUpperCase();
			M.hasContent = !t0.test(M.type);
			if (W && n.active++ === 0) {
				n.event.trigger("ajaxStart");
			}
			if (!M.hasContent) {
				if (M.data) {
					M.url += (R.test(M.url) ? "&" : "?") + M.data;
					delete M.data;
				}
				K = M.url;
				if (M.cache === false) {
					var J = n.now(),
					b = M.url.replace(Z1, "$1_=" + J);
					M.url = b + ((b === M.url) ? (R.test(M.url) ? "&" : "?") + "_=" + J : "");
				}
			}
			if (M.data && M.hasContent && M.contentType !== false || Z.contentType) {
				P.setRequestHeader("Content-Type", M.contentType);
			}
			if (M.ifModified) {
				K = K || M.url;
				if (n.lastModified[K]) {
					P.setRequestHeader("If-Modified-Since", n.lastModified[K]);
				}
				if (n.etag[K]) {
					P.setRequestHeader("If-None-Match", n.etag[K]);
				}
			}
			P.setRequestHeader("Accept", M.dataTypes[0] && M.accepts[M.dataTypes[0]] ? M.accepts[M.dataTypes[0]] + (M.dataTypes[0] !== "*" ? ", " + j0 + "; q=0.01" : "") : M.accepts["*"]);
			for (I in M.headers) {
				P.setRequestHeader(I, M.headers[I]);
			}
			if (M.beforeSend && (M.beforeSend.call(T, P, M) === false || a === 2)) {
				P.abort();
				return false;
			}
			for (I in {
				success : 1,
				error : 1,
				complete : 1
			}) {
				P[I](M[I]);
			}
			G = Y(C, M, Z, P);
			if (!G) {
				E(-1, "No Transport");
			} else {
				P.readyState = 1;
				if (W) {
					A.trigger("ajaxSend", [P, M]);
				}
				if (M.async && M.timeout > 0) {
					F = setTimeout(function () {
							P.abort("timeout");
						}, M.timeout);
				}
				try {
					a = 1;
					G.send(Q, E);
				} catch (c) {
					if (a < 2) {
						E(-1, c);
					} else {
						throw c;
					}
				}
			}
			return P;
		},
		param : function (B, _) {
			var D = [],
			A = function (_, A) {
				A = n.isFunction(A) ? A() : A;
				D[D.length] = encodeURIComponent(_) + "=" + encodeURIComponent(A);
			};
			if (_ === C1) {
				_ = n.ajaxSettings.traditional;
			}
			if (n.isArray(B) || (B.jquery && !n.isPlainObject(B))) {
				n.each(B, function () {
					A(this.name, this.value);
				});
			} else {
				for (var C in B) {
					t(C, B[C], _, A);
				}
			}
			return D.join("&").replace(c, "+");
		}
	});
	function t(C, B, _, A) {
		if (n.isArray(B)) {
			n.each(B, function (B, D) {
				if (_ || G0.test(C)) {
					A(C, D);
				} else {
					t(C + "[" + (typeof D === "object" ? B : "") + "]", D, _, A);
				}
			});
		} else {
			if (!_ && n.type(B) === "object") {
				for (var D in B) {
					t(C + "[" + D + "]", B[D], _, A);
				}
			} else {
				A(C, B);
			}
		}
	}
	n.extend({
		active : 0,
		lastModified : {},
		etag : {}

	});
	function R0(H, B, A) {
		var _ = H.contents,
		F = H.dataTypes,
		E = H.responseFields,
		G,
		C,
		D,
		I;
		for (C in E) {
			if (C in A) {
				B[E[C]] = A[C];
			}
		}
		while (F[0] === "*") {
			F.shift();
			if (G === C1) {
				G = H.mimeType || B.getResponseHeader("content-type");
			}
		}
		if (G) {
			for (C in _) {
				if (_[C] && _[C].test(G)) {
					F.unshift(C);
					break;
				}
			}
		}
		if (F[0]in A) {
			D = F[0];
		} else {
			for (C in A) {
				if (!F[0] || H.converters[C + " " + F[0]]) {
					D = C;
					break;
				}
				if (!I) {
					I = C;
				}
			}
			D = D || I;
		}
		if (D) {
			if (D !== F[0]) {
				F.unshift(D);
			}
			return A[D];
		}
	}
	function U(L, F) {
		if (L.dataFilter) {
			F = L.dataFilter(F, L.dataType);
		}
		var E = L.dataTypes,
		A = {},
		H,
		G,
		B = E.length,
		I,
		_ = E[0],
		J,
		M,
		C,
		K,
		D;
		for (H = 1; H < B; H++) {
			if (H === 1) {
				for (G in L.converters) {
					if (typeof G === "string") {
						A[G.toLowerCase()] = L.converters[G];
					}
				}
			}
			J = _;
			_ = E[H];
			if (_ === "*") {
				_ = J;
			} else {
				if (J !== "*" && J !== _) {
					M = J + " " + _;
					C = A[M] || A["* " + _];
					if (!C) {
						D = C1;
						for (K in A) {
							I = K.split(" ");
							if (I[0] === J || I[0] === "*") {
								D = A[I[1] + " " + _];
								if (D) {
									K = A[K];
									if (K === true) {
										C = D;
									} else {
										if (D === true) {
											C = K;
										}
									}
									break;
								}
							}
						}
					}
					if (!(C || D)) {
						n.error("No conversion from " + M.replace(" ", " to "));
					}
					if (C !== true) {
						F = C ? C(F) : D(K(F));
					}
				}
			}
		}
		return F;
	}
	var g = n.now(),
	v = /(\=)\?(&|$)|\?\?/i;
	n.ajaxSetup({
		jsonp : "callback",
		jsonpCallback : function () {
			return n.expando + "_" + (g++);
		}
	});
	n.ajaxPrefilter("json jsonp", function (I, C, _) {
		var A = (typeof I.data === "string") && /^application\/x\-www\-form\-urlencoded/.test(I.contentType);
		if (I.dataTypes[0] === "jsonp" || I.jsonp !== false && (v.test(I.url) || A && v.test(I.data))) {
			var B,
			H = I.jsonpCallback = n.isFunction(I.jsonpCallback) ? I.jsonpCallback() : I.jsonpCallback,
			E = N0[H],
			F = I.url,
			D = I.data,
			G = "$1" + H + "$2";
			if (I.jsonp !== false) {
				F = F.replace(v, G);
				if (I.url === F) {
					if (A) {
						D = D.replace(v, G);
					}
					if (I.data === D) {
						F += (/\?/.test(F) ? "&" : "?") + I.jsonp + "=" + H;
					}
				}
			}
			I.url = F;
			I.data = D;
			N0[H] = function (_) {
				B = [_];
			};
			_.always(function () {
				N0[H] = E;
				if (B && n.isFunction(E)) {
					N0[H](B[0]);
				}
			});
			I.converters["script json"] = function () {
				if (!B) {
					n.error(H + " was not called");
				}
				return B[0];
			};
			I.dataTypes[0] = "json";
			return "script";
		}
	});
	n.ajaxSetup({
		accepts : {
			script : "text/javascript, application/javascript, application/ecmascript, application/x-ecmascript"
		},
		contents : {
			script : /javascript|ecmascript/
		},
		converters : {
			"text script" : function (_) {
				n.globalEval(_);
				return _;
			}
		}
	});
	n.ajaxPrefilter("script", function (_) {
		if (_.cache === C1) {
			_.cache = false;
		}
		if (_.crossDomain) {
			_.type = "GET";
			_.global = false;
		}
	});
	n.ajaxTransport("script", function (B) {
		if (B.crossDomain) {
			var _,
			A = y0.head || y0.getElementsByTagName("head")[0] || y0.documentElement;
			return {
				send : function (D, C) {
					_ = y0.createElement("script");
					_.async = "async";
					if (B.scriptCharset) {
						_.charset = B.scriptCharset;
					}
					_.src = B.url;
					_.onload = _.onreadystatechange = function (D, B) {
						if (B || !_.readyState || /loaded|complete/.test(_.readyState)) {
							_.onload = _.onreadystatechange = null;
							if (A && _.parentNode) {
								A.removeChild(_);
							}
							_ = C1;
							if (!B) {
								C(200, "success");
							}
						}
					};
					A.insertBefore(_, A.firstChild);
				},
				abort : function () {
					if (_) {
						_.onload(0, 1);
					}
				}
			};
		}
	});
	var J = N0.ActiveXObject ? function () {
		for (var _ in z0) {
			z0[_](0, 1);
		}
	}
	 : false,
	m = 0,
	z0;
	function J1() {
		try {
			return new N0.XMLHttpRequest();
		} catch (_) {}

	}
	function v0() {
		try {
			return new N0.ActiveXObject("Microsoft.XMLHTTP");
		} catch (_) {}

	}
	n.ajaxSettings.xhr = N0.ActiveXObject ? function () {
		return !this.isLocal && J1() || v0();
	}
	 : J1;
	(function (_) {
		n.extend(n.support, {
			ajax : !!_,
			cors : !!_ && ("withCredentials" in _)
		});
	})(n.ajaxSettings.xhr());
	if (n.support.ajax) {
		n.ajaxTransport(function (A) {
			if (!A.crossDomain || n.support.cors) {
				var _;
				return {
					send : function (E, G) {
						var D = A.xhr(),
						B,
						C;
						if (A.username) {
							D.open(A.type, A.url, A.async, A.username, A.password);
						} else {
							D.open(A.type, A.url, A.async);
						}
						if (A.xhrFields) {
							for (C in A.xhrFields) {
								D[C] = A.xhrFields[C];
							}
						}
						if (A.mimeType && D.overrideMimeType) {
							D.overrideMimeType(A.mimeType);
						}
						if (!A.crossDomain && !E["X-Requested-With"]) {
							E["X-Requested-With"] = "XMLHttpRequest";
						}
						try {
							for (C in E) {
								D.setRequestHeader(C, E[C]);
							}
						} catch (F) {}

						D.send((A.hasContent && A.data) || null);
						_ = function (I, E) {
							var K,
							F,
							N,
							C,
							L;
							try {
								if (_ && (E || D.readyState === 4)) {
									_ = C1;
									if (B) {
										D.onreadystatechange = n.noop;
										if (J) {
											delete z0[B];
										}
									}
									if (E) {
										if (D.readyState !== 4) {
											D.abort();
										}
									} else {
										K = D.status;
										N = D.getAllResponseHeaders();
										C = {};
										L = D.responseXML;
										if (L && L.documentElement) {
											C.xml = L;
										}
										try {
											C.text = D.responseText;
										} catch (I) {}

										try {
											F = D.statusText;
										} catch (M) {
											F = "";
										}
										if (!K && A.isLocal && !A.crossDomain) {
											K = C.text ? 200 : 404;
										} else {
											if (K === 1223) {
												K = 204;
											}
										}
									}
								}
							} catch (H) {
								if (!E) {
									G(-1, H);
								}
							}
							if (C) {
								G(K, F, C, N);
							}
						};
						if (!A.async || D.readyState === 4) {
							_();
						} else {
							B = ++m;
							if (J) {
								if (!z0) {
									z0 = {};
									n(N0).unload(J);
								}
								z0[B] = _;
							}
							D.onreadystatechange = _;
						}
					},
					abort : function () {
						if (_) {
							_(0, 1);
						}
					}
				};
			}
		});
	}
	var W = {},
	V0,
	D,
	s = /^(?:toggle|show|hide)$/,
	w = /^([+\-]=)?([\d+.\-]+)([a-z%]*)$/i,
	n0,
	K1 = [["height", "marginTop", "marginBottom", "paddingTop", "paddingBottom"], ["width", "marginLeft", "marginRight", "paddingLeft", "paddingRight"], ["opacity"]],
	M1;
	n.fn.extend({
		show : function (B, F, D) {
			var C,
			_;
			if (B || B === 0) {
				return this.animate(Y0("show", 3), B, F, D);
			} else {
				for (var A = 0, E = this.length; A < E; A++) {
					C = this[A];
					if (C.style) {
						_ = C.style.display;
						if (!n._data(C, "olddisplay") && _ === "none") {
							_ = C.style.display = "";
						}
						if ((_ === "" && n.css(C, "display") === "none") || !n.contains(C.ownerDocument.documentElement, C)) {
							n._data(C, "olddisplay", X0(C.nodeName));
						}
					}
				}
				for (A = 0; A < E; A++) {
					C = this[A];
					if (C.style) {
						_ = C.style.display;
						if (_ === "" || _ === "none") {
							C.style.display = n._data(C, "olddisplay") || "";
						}
					}
				}
				return this;
			}
		},
		hide : function (B, F, D) {
			if (B || B === 0) {
				return this.animate(Y0("hide", 3), B, F, D);
			} else {
				var C,
				_,
				A = 0,
				E = this.length;
				for (; A < E; A++) {
					C = this[A];
					if (C.style) {
						_ = n.css(C, "display");
						if (_ !== "none" && !n._data(C, "olddisplay")) {
							n._data(C, "olddisplay", _);
						}
					}
				}
				for (A = 0; A < E; A++) {
					if (this[A].style) {
						this[A].style.display = "none";
					}
				}
				return this;
			}
		},
		_toggle : n.fn.toggle,
		toggle : function (C, A, _) {
			var B = typeof C === "boolean";
			if (n.isFunction(C) && n.isFunction(A)) {
				this._toggle.apply(this, arguments);
			} else {
				if (C == null || B) {
					this.each(function () {
						var _ = B ? C : n(this).is(":hidden");
						n(this)[_ ? "show" : "hide"]();
					});
				} else {
					this.animate(Y0("toggle", 3), C, A, _);
				}
			}
			return this;
		},
		fadeTo : function (A, _, C, B) {
			return this.filter(":hidden").css("opacity", 0).show().end().animate({
				opacity : _
			}, A, C, B);
		},
		animate : function (B, _, D, A) {
			var E = n.speed(_, D, A);
			if (n.isEmptyObject(B)) {
				return this.each(E.complete, [false]);
			}
			B = n.extend({}, B);
			function C() {
				if (E.queue === false) {
					n._mark(this);
				}
				var K = n.extend({}, E),
				C = this.nodeType === 1,
				J = C && n(this).is(":hidden"),
				M,
				A,
				I,
				O,
				G,
				L,
				_,
				D,
				H,
				F,
				N;
				K.animatedProperties = {};
				for (I in B) {
					M = n.camelCase(I);
					if (I !== M) {
						B[M] = B[I];
						delete B[I];
					}
					if ((G = n.cssHooks[M]) && "expand" in G) {
						L = G.expand(B[M]);
						delete B[M];
						for (I in L) {
							if (!(I in B)) {
								B[I] = L[I];
							}
						}
					}
				}
				for (M in B) {
					A = B[M];
					if (n.isArray(A)) {
						K.animatedProperties[M] = A[1];
						A = B[M] = A[0];
					} else {
						K.animatedProperties[M] = K.specialEasing && K.specialEasing[M] || K.easing || "swing";
					}
					if (A === "hide" && J || A === "show" && !J) {
						return K.complete.call(this);
					}
					if (C && (M === "height" || M === "width")) {
						K.overflow = [this.style.overflow, this.style.overflowX, this.style.overflowY];
						if (n.css(this, "display") === "inline" && n.css(this, "float") === "none") {
							if (!n.support.inlineBlockNeedsLayout || X0(this.nodeName) === "inline") {
								this.style.display = "inline-block";
							} else {
								this.style.zoom = 1;
							}
						}
					}
				}
				if (K.overflow != null) {
					this.style.overflow = "hidden";
				}
				for (I in B) {
					O = new n.fx(this, K, I);
					A = B[I];
					if (s.test(A)) {
						N = n._data(this, "toggle" + I) || (A === "toggle" ? J ? "show" : "hide" : 0);
						if (N) {
							n._data(this, "toggle" + I, N === "show" ? "hide" : "show");
							O[N]();
						} else {
							O[A]();
						}
					} else {
						_ = w.exec(A);
						D = O.cur();
						if (_) {
							H = parseFloat(_[2]);
							F = _[3] || (n.cssNumber[I] ? "" : "px");
							if (F !== "px") {
								n.style(this, I, (H || 1) + F);
								D = ((H || 1) / O.cur()) * D;
								n.style(this, I, D + F);
							}
							if (_[1]) {
								H = ((_[1] === "-=" ? -1 : 1) * H) + D;
							}
							O.custom(D, H, F);
						} else {
							O.custom(D, A, "");
						}
					}
				}
				return true;
			}
			return E.queue === false ? this.each(C) : this.queue(E.queue, C);
		},
		stop : function (A, _, B) {
			if (typeof A !== "string") {
				B = _;
				_ = A;
				A = C1;
			}
			if (_ && A !== false) {
				this.queue(A || "fx", []);
			}
			return this.each(function () {
				var _,
				D = false,
				F = n.timers,
				C = n._data(this);
				if (!B) {
					n._unmark(true, this);
				}
				function E(A, D, _) {
					var C = D[_];
					n.removeData(A, _, true);
					C.stop(B);
				}
				if (A == null) {
					for (_ in C) {
						if (C[_] && C[_].stop && _.indexOf(".run") === _.length - 4) {
							E(this, C, _);
						}
					}
				} else {
					if (C[_ = A + ".run"] && C[_].stop) {
						E(this, C, _);
					}
				}
				for (_ = F.length; _--; ) {
					if (F[_].elem === this && (A == null || F[_].queue === A)) {
						if (B) {
							F[_](true);
						} else {
							F[_].saveState();
						}
						D = true;
						F.splice(_, 1);
					}
				}
				if (!(B && D)) {
					n.dequeue(this, A);
				}
			});
		}
	});
	function d0() {
		setTimeout(P1, 0);
		return (M1 = n.now());
	}
	function P1() {
		M1 = C1;
	}
	function Y0(B, _) {
		var A = {};
		n.each(K1.concat.apply([], K1.slice(0, _)), function () {
			A[this] = B;
		});
		return A;
	}
	n.each({
		slideDown : Y0("show", 1),
		slideUp : Y0("hide", 1),
		slideToggle : Y0("toggle", 1),
		fadeIn : {
			opacity : "show"
		},
		fadeOut : {
			opacity : "hide"
		},
		fadeToggle : {
			opacity : "toggle"
		}
	}, function (A, _) {
		n.fn[A] = function (A, C, B) {
			return this.animate(_, A, C, B);
		};
	});
	n.extend({
		speed : function (_, B, C) {
			var A = _ && typeof _ === "object" ? n.extend({}, _) : {
				complete : C || !C && B || n.isFunction(_) && _,
				duration : _,
				easing : C && B || B && !n.isFunction(B) && B
			};
			A.duration = n.fx.off ? 0 : typeof A.duration === "number" ? A.duration : A.duration in n.fx.speeds ? n.fx.speeds[A.duration] : n.fx.speeds._default;
			if (A.queue == null || A.queue === true) {
				A.queue = "fx";
			}
			A.old = A.complete;
			A.complete = function (_) {
				if (n.isFunction(A.old)) {
					A.old.call(this);
				}
				if (A.queue) {
					n.dequeue(this, A.queue);
				} else {
					if (_ !== false) {
						n._unmark(this);
					}
				}
			};
			return A;
		},
		easing : {
			linear : function (_) {
				return _;
			},
			swing : function (_) {
				return (-Math.cos(_ * Math.PI) / 2) + 0.5;
			}
		},
		timers : [],
		fx : function (_, B, A) {
			this.options = B;
			this.elem = _;
			this.prop = A;
			B.orig = B.orig || {};
		}
	});
	n.fx.prototype = {
		update : function () {
			if (this.options.step) {
				this.options.step.call(this.elem, this.now, this);
			}
			(n.fx.step[this.prop] || n.fx.step._default)(this);
		},
		cur : function () {
			if (this.elem[this.prop] != null && (!this.elem.style || this.elem.style[this.prop] == null)) {
				return this.elem[this.prop];
			}
			var A,
			_ = n.css(this.elem, this.prop);
			return isNaN(A = parseFloat(_)) ? !_ || _ === "auto" ? 0 : _ : A;
		},
		custom : function (E, _, C) {
			var B = this,
			A = n.fx;
			this.startTime = M1 || d0();
			this.end = _;
			this.now = this.start = E;
			this.pos = this.state = 0;
			this.unit = C || this.unit || (n.cssNumber[this.prop] ? "" : "px");
			function D(_) {
				return B.step(_);
			}
			D.queue = this.options.queue;
			D.elem = this.elem;
			D.saveState = function () {
				if (n._data(B.elem, "fxshow" + B.prop) === C1) {
					if (B.options.hide) {
						n._data(B.elem, "fxshow" + B.prop, B.start);
					} else {
						if (B.options.show) {
							n._data(B.elem, "fxshow" + B.prop, B.end);
						}
					}
				}
			};
			if (D() && n.timers.push(D) && !n0) {
				n0 = setInterval(A.tick, A.interval);
			}
		},
		show : function () {
			var _ = n._data(this.elem, "fxshow" + this.prop);
			this.options.orig[this.prop] = _ || n.style(this.elem, this.prop);
			this.options.show = true;
			if (_ !== C1) {
				this.custom(this.cur(), _);
			} else {
				this.custom(this.prop === "width" || this.prop === "height" ? 1 : 0, this.cur());
			}
			n(this.elem).show();
		},
		hide : function () {
			this.options.orig[this.prop] = n._data(this.elem, "fxshow" + this.prop) || n.style(this.elem, this.prop);
			this.options.hide = true;
			this.custom(this.cur(), 0);
		},
		step : function (D) {
			var C,
			E,
			F,
			B = M1 || d0(),
			A = true,
			_ = this.elem,
			G = this.options;
			if (D || B >= G.duration + this.startTime) {
				this.now = this.end;
				this.pos = this.state = 1;
				this.update();
				G.animatedProperties[this.prop] = true;
				for (C in G.animatedProperties) {
					if (G.animatedProperties[C] !== true) {
						A = false;
					}
				}
				if (A) {
					if (G.overflow != null && !n.support.shrinkWrapBlocks) {
						n.each(["", "X", "Y"], function (A, B) {
							_.style["overflow" + B] = G.overflow[A];
						});
					}
					if (G.hide) {
						n(_).hide();
					}
					if (G.hide || G.show) {
						for (C in G.animatedProperties) {
							n.style(_, C, G.orig[C]);
							n.removeData(_, "fxshow" + C, true);
							n.removeData(_, "toggle" + C, true);
						}
					}
					F = G.complete;
					if (F) {
						G.complete = false;
						F.call(_);
					}
				}
				return false;
			} else {
				if (G.duration == Infinity) {
					this.now = B;
				} else {
					E = B - this.startTime;
					this.state = E / G.duration;
					this.pos = n.easing[G.animatedProperties[this.prop]](this.state, E, 0, 1, G.duration);
					this.now = this.start + ((this.end - this.start) * this.pos);
				}
				this.update();
			}
			return true;
		}
	};
	n.extend(n.fx, {
		tick : function () {
			var A,
			B = n.timers,
			_ = 0;
			for (; _ < B.length; _++) {
				A = B[_];
				if (!A() && B[_] === A) {
					B.splice(_--, 1);
				}
			}
			if (!B.length) {
				n.fx.stop();
			}
		},
		interval : 13,
		stop : function () {
			clearInterval(n0);
			n0 = null;
		},
		speeds : {
			slow : 600,
			fast : 200,
			_default : 400
		},
		step : {
			opacity : function (_) {
				n.style(_.elem, "opacity", _.now);
			},
			_default : function (_) {
				if (_.elem.style && _.elem.style[_.prop] != null) {
					_.elem.style[_.prop] = _.now + _.unit;
				} else {
					_.elem[_.prop] = _.now;
				}
			}
		}
	});
	n.each(K1.concat.apply([], K1), function (_, A) {
		if (A.indexOf("margin")) {
			n.fx.step[A] = function (_) {
				n.style(_.elem, A, Math.max(0, _.now) + _.unit);
			};
		}
	});
	if (n.expr && n.expr.filters) {
		n.expr.filters.animated = function (_) {
			return n.grep(n.timers, function (A) {
				return _ === A.elem;
			}).length;
		};
	}
	function X0(B) {
		if (!W[B]) {
			var C = y0.body,
			A = n("<" + B + ">").appendTo(C),
			_ = A.css("display");
			A.remove();
			if (_ === "none" || _ === "") {
				if (!V0) {
					V0 = y0.createElement("iframe");
					V0.frameBorder = V0.width = V0.height = 0;
				}
				C.appendChild(V0);
				if (!D || !V0.createElement) {
					D = (V0.contentWindow || V0.contentDocument).document;
					D.write((n.support.boxModel ? "<!doctype html>" : "") + "<html><body>");
					D.close();
				}
				A = D.createElement(B);
				D.body.appendChild(A);
				_ = n.css(A, "display");
				C.removeChild(V0);
			}
			W[B] = _;
		}
		return W[B];
	}
	var D0,
	H0 = /^t(?:able|d|h)$/i,
	X = /^(?:body|html)$/i;
	if ("getBoundingClientRect" in y0.documentElement) {
		D0 = function (_, H, A, D) {
			try {
				D = _.getBoundingClientRect();
			} catch (L) {}

			if (!D || !n.contains(A, _)) {
				return D ? {
					top : D.top,
					left : D.left
				}
				 : {
					top : 0,
					left : 0
				};
			}
			var J = H.body,
			K = S1(H),
			F = A.clientTop || J.clientTop || 0,
			B = A.clientLeft || J.clientLeft || 0,
			G = K.pageYOffset || n.support.boxModel && A.scrollTop || J.scrollTop,
			E = K.pageXOffset || n.support.boxModel && A.scrollLeft || J.scrollLeft,
			C = D.top + G - F,
			I = D.left + E - B;
			return {
				top : C,
				left : I
			};
		};
	} else {
		D0 = function (B, _, A) {
			var D,
			G = B.offsetParent,
			H = B,
			I = _.body,
			J = _.defaultView,
			F = J ? J.getComputedStyle(B, null) : B.currentStyle,
			C = B.offsetTop,
			E = B.offsetLeft;
			while ((B = B.parentNode) && B !== I && B !== A) {
				if (n.support.fixedPosition && F.position === "fixed") {
					break;
				}
				D = J ? J.getComputedStyle(B, null) : B.currentStyle;
				C -= B.scrollTop;
				E -= B.scrollLeft;
				if (B === G) {
					C += B.offsetTop;
					E += B.offsetLeft;
					if (n.support.doesNotAddBorder && !(n.support.doesAddBorderForTableAndCells && H0.test(B.nodeName))) {
						C += parseFloat(D.borderTopWidth) || 0;
						E += parseFloat(D.borderLeftWidth) || 0;
					}
					H = G;
					G = B.offsetParent;
				}
				if (n.support.subtractsBorderForOverflowNotVisible && D.overflow !== "visible") {
					C += parseFloat(D.borderTopWidth) || 0;
					E += parseFloat(D.borderLeftWidth) || 0;
				}
				F = D;
			}
			if (F.position === "relative" || F.position === "static") {
				C += I.offsetTop;
				E += I.offsetLeft;
			}
			if (n.support.fixedPosition && F.position === "fixed") {
				C += Math.max(A.scrollTop, I.scrollTop);
				E += Math.max(A.scrollLeft, I.scrollLeft);
			}
			return {
				top : C,
				left : E
			};
		};
	}
	n.fn.offset = function (B) {
		if (arguments.length) {
			return B === C1 ? this : this.each(function (_) {
				n.offset.setOffset(this, B, _);
			});
		}
		var A = this[0],
		_ = A && A.ownerDocument;
		if (!_) {
			return null;
		}
		if (A === _.body) {
			return n.offset.bodyOffset(A);
		}
		return D0(A, _, _.documentElement);
	};
	n.offset = {
		bodyOffset : function (B) {
			var _ = B.offsetTop,
			A = B.offsetLeft;
			if (n.support.doesNotIncludeMarginInBodyOffset) {
				_ += parseFloat(n.css(B, "marginTop")) || 0;
				A += parseFloat(n.css(B, "marginLeft")) || 0;
			}
			return {
				top : _,
				left : A
			};
		},
		setOffset : function (A, G, I) {
			var C = n.css(A, "position");
			if (C === "static") {
				A.style.position = "relative";
			}
			var _ = n(A),
			J = _.offset(),
			L = n.css(A, "top"),
			E = n.css(A, "left"),
			H = (C === "absolute" || C === "fixed") && n.inArray("auto", [L, E]) > -1,
			B = {},
			F = {},
			K,
			D;
			if (H) {
				F = _.position();
				K = F.top;
				D = F.left;
			} else {
				K = parseFloat(L) || 0;
				D = parseFloat(E) || 0;
			}
			if (n.isFunction(G)) {
				G = G.call(A, I, J);
			}
			if (G.top != null) {
				B.top = (G.top - J.top) + K;
			}
			if (G.left != null) {
				B.left = (G.left - J.left) + D;
			}
			if ("using" in G) {
				G.using.call(A, B);
			} else {
				_.css(B);
			}
		}
	};
	n.fn.extend({
		position : function () {
			if (!this[0]) {
				return null;
			}
			var _ = this[0],
			A = this.offsetParent(),
			C = this.offset(),
			B = X.test(A[0].nodeName) ? {
				top : 0,
				left : 0
			}
			 : A.offset();
			C.top -= parseFloat(n.css(_, "marginTop")) || 0;
			C.left -= parseFloat(n.css(_, "marginLeft")) || 0;
			B.top += parseFloat(n.css(A[0], "borderTopWidth")) || 0;
			B.left += parseFloat(n.css(A[0], "borderLeftWidth")) || 0;
			return {
				top : C.top - B.top,
				left : C.left - B.left
			};
		},
		offsetParent : function () {
			return this.map(function () {
				var _ = this.offsetParent || y0.body;
				while (_ && (!X.test(_.nodeName) && n.css(_, "position") === "static")) {
					_ = _.offsetParent;
				}
				return _;
			});
		}
	});
	n.each({
		scrollLeft : "pageXOffset",
		scrollTop : "pageYOffset"
	}, function (B, A) {
		var _ = /Y/.test(A);
		n.fn[B] = function (C) {
			return n.access(this, function (C, E, B) {
				var D = S1(C);
				if (B === C1) {
					return D ? (A in D) ? D[A] : n.support.boxModel && D.document.documentElement[E] || D.document.body[E] : C[E];
				}
				if (D) {
					D.scrollTo(!_ ? B : n(D).scrollLeft(), _ ? B : n(D).scrollTop());
				} else {
					C[E] = B;
				}
			}, B, C, arguments.length, null);
		};
	});
	function S1(_) {
		return n.isWindow(_) ? _ : _.nodeType === 9 ? _.defaultView || _.parentWindow : false;
	}
	n.each({
		Height : "height",
		Width : "width"
	}, function (C, A) {
		var D = "client" + C,
		B = "scroll" + C,
		_ = "offset" + C;
		n.fn["inner" + C] = function () {
			var _ = this[0];
			return _ ? _.style ? parseFloat(n.css(_, A, "padding")) : this[A]() : null;
		};
		n.fn["outer" + C] = function (B) {
			var _ = this[0];
			return _ ? _.style ? parseFloat(n.css(_, A, B ? "margin" : "border")) : this[A]() : null;
		};
		n.fn[A] = function (C) {
			return n.access(this, function (E, F, C) {
				var A,
				H,
				G,
				I;
				if (n.isWindow(E)) {
					A = E.document;
					H = A.documentElement[D];
					return n.support.boxModel && H || A.body && A.body[D] || H;
				}
				if (E.nodeType === 9) {
					A = E.documentElement;
					if (A[D] >= A[B]) {
						return A[D];
					}
					return Math.max(E.body[B], A[B], E.body[_], A[_]);
				}
				if (C === C1) {
					G = n.css(E, F);
					I = parseFloat(G);
					return n.isNumeric(I) ? I : G;
				}
				n(E).css(F, C);
			}, A, C, arguments.length, null);
		};
	});
	N0.jQuery = N0.$ = n;
	if (typeof define === "function" && define.amd && define.amd.jQuery) {
		define("jquery", [], function () {
			return n;
		});
	}
})(window);
(function () {
	var k = this,
	N = k._,
	i = {},
	W = Array.prototype,
	I = Object.prototype,
	Q = Function.prototype,
	R = W.push,
	M = W.slice,
	K = W.concat,
	f = W.unshift,
	Z = I.toString,
	B = I.hasOwnProperty,
	C = W.forEach,
	F = W.map,
	S = W.reduce,
	H = W.reduceRight,
	e = W.filter,
	a = W.every,
	h = W.some,
	c = W.indexOf,
	V = W.lastIndexOf,
	l = Array.isArray,
	T = Object.keys,
	j = Q.bind,
	g = function (_) {
		if (_ instanceof g) {
			return _;
		}
		if (!(this instanceof g)) {
			return new g(_);
		}
		this._wrapped = _;
	};
	typeof exports != "undefined" ? (typeof module != "undefined" && module.exports && (exports = module.exports = g), exports._ = g) : k._ = g,
	g.VERSION = "1.4.2";
	var P = g.each = g.forEach = function (F, D, B) {
		if (F == null) {
			return;
		}
		if (C && F.forEach === C) {
			F.forEach(D, B);
		} else {
			if (F.length === +F.length) {
				for (var A = 0, E = F.length; A < E; A++) {
					if (D.call(B, F[A], A, F) === i) {
						return;
					}
				}
			} else {
				for (var _ in F) {
					if (g.has(F, _) && D.call(B, F[_], _, F) === i) {
						return;
					}
				}
			}
		}
	};
	g.map = g.collect = function (C, A, B) {
		var _ = [];
		return C == null ? _ : F && C.map === F ? C.map(A, B) : (P(C, function (E, C, D) {
				_[_.length] = A.call(B, E, C, D);
			}), _);
	},
	g.reduce = g.foldl = g.inject = function (D, B, C, A) {
		var _ = arguments.length > 2;
		D == null && (D = []);
		if (S && D.reduce === S) {
			return A && (B = g.bind(B, A)),
			_ ? D.reduce(B, C) : D.reduce(B);
		}
		P(D, function (F, E, D) {
			_ ? C = B.call(A, C, F, E, D) : (C = F, _ = !0);
		});
		if (!_) {
			throw new TypeError("Reduce of empty array with no initial value");
		}
		return C;
	},
	g.reduceRight = g.foldr = function (F, C, D, B) {
		var A = arguments.length > 2;
		F == null && (F = []);
		if (H && F.reduceRight === H) {
			return B && (C = g.bind(C, B)),
			arguments.length > 2 ? F.reduceRight(C, D) : F.reduceRight(C);
		}
		var E = F.length;
		if (E !== +E) {
			var _ = g.keys(F);
			E = _.length;
		}
		P(F, function (I, G, H) {
			G = _ ? _[--E] : --E,
			A ? D = C.call(B, D, F[G], G, H) : (D = F[G], A = !0);
		});
		if (!A) {
			throw new TypeError("Reduce of empty array with no initial value");
		}
		return D;
	},
	g.find = g.detect = function (C, A, B) {
		var _;
		return X(C, function (E, C, D) {
			if (A.call(B, E, C, D)) {
				return _ = E,
				!0;
			}
		}),
		_;
	},
	g.filter = g.select = function (C, A, B) {
		var _ = [];
		return C == null ? _ : e && C.filter === e ? C.filter(A, B) : (P(C, function (E, C, D) {
				A.call(B, E, C, D) && (_[_.length] = E);
			}), _);
	},
	g.reject = function (C, A, B) {
		var _ = [];
		return C == null ? _ : (P(C, function (E, C, D) {
				A.call(B, E, C, D) || (_[_.length] = E);
			}), _);
	},
	g.every = g.all = function (C, B, A) {
		B || (B = g.identity);
		var _ = !0;
		return C == null ? _ : a && C.every === a ? C.every(B, A) : (P(C, function (E, D, C) {
				if (!(_ = _ && B.call(A, E, D, C))) {
					return i;
				}
			}), !!_);
	};
	var X = g.some = g.any = function (C, B, A) {
		B || (B = g.identity);
		var _ = !1;
		return C == null ? _ : h && C.some === h ? C.some(B, A) : (P(C, function (E, D, C) {
				if (_ || (_ = B.call(A, E, D, C))) {
					return i;
				}
			}), !!_);
	};
	g.contains = g.include = function (B, _) {
		var A = !1;
		return B == null ? A : c && B.indexOf === c ? B.indexOf(_) != -1 : (A = X(B, function (A) {
					return A === _;
				}), A);
	},
	g.invoke = function (B, _) {
		var A = M.call(arguments, 2);
		return g.map(B, function (B) {
			return (g.isFunction(_) ? _ : B[_]).apply(B, A);
		});
	},
	g.pluck = function (A, _) {
		return g.map(A, function (A) {
			return A[_];
		});
	},
	g.where = function (A, _) {
		return g.isEmpty(_) ? [] : g.filter(A, function (B) {
			for (var A in _) {
				if (_[A] !== B[A]) {
					return !1;
				}
			}
			return !0;
		});
	},
	g.max = function (C, A, B) {
		if (!A && g.isArray(C) && C[0] === +C[0] && C.length < 65535) {
			return Math.max.apply(Math, C);
		}
		if (!A && g.isEmpty(C)) {
			return -Infinity;
		}
		var _ = {
			computed : -Infinity
		};
		return P(C, function (F, D, E) {
			var C = A ? A.call(B, F, D, E) : F;
			C >= _.computed && (_ = {
					value : F,
					computed : C
				});
		}),
		_.value;
	},
	g.min = function (C, A, B) {
		if (!A && g.isArray(C) && C[0] === +C[0] && C.length < 65535) {
			return Math.min.apply(Math, C);
		}
		if (!A && g.isEmpty(C)) {
			return Infinity;
		}
		var _ = {
			computed : Infinity
		};
		return P(C, function (F, D, E) {
			var C = A ? A.call(B, F, D, E) : F;
			C < _.computed && (_ = {
					value : F,
					computed : C
				});
		}),
		_.value;
	},
	g.shuffle = function (C) {
		var A,
		B = 0,
		_ = [];
		return P(C, function (C) {
			A = g.random(B++),
			_[B - 1] = _[A],
			_[A] = C;
		}),
		_;
	};
	var A = function (_) {
		return g.isFunction(_) ? _ : function (A) {
			return A[_];
		};
	};
	g.sortBy = function (D, B, C) {
		var _ = A(B);
		return g.pluck(g.map(D, function (D, B, A) {
				return {
					value : D,
					index : B,
					criteria : _.call(C, D, B, A)
				};
			}).sort(function (C, A) {
				var B = C.criteria,
				_ = A.criteria;
				if (B !== _) {
					if (B > _ || B === void 0) {
						return 1;
					}
					if (B < _ || _ === void 0) {
						return -1;
					}
				}
				return C.index < A.index ? -1 : 1;
			}), "value");
	};
	var b = function (F, C, D, B) {
		var _ = {},
		E = A(C);
		return P(F, function (G, A) {
			var C = E.call(D, G, A, F);
			B(_, C, G);
		}),
		_;
	};
	g.groupBy = function (B, _, A) {
		return b(B, _, A, function (B, _, A) {
			(g.has(B, _) ? B[_] : B[_] = []).push(A);
		});
	},
	g.countBy = function (B, _, A) {
		return b(B, _, A, function (B, _, A) {
			g.has(B, _) || (B[_] = 0),
			B[_]++;
		});
	},
	g.sortedIndex = function (H, E, F, C) {
		F = F == null ? g.identity : A(F);
		var B = F.call(C, E),
		G = 0,
		_ = H.length;
		while (G < _) {
			var D = G + _ >>> 1;
			F.call(C, H[D]) < B ? G = D + 1 : _ = D;
		}
		return G;
	},
	g.toArray = function (_) {
		return _ ? _.length === +_.length ? M.call(_) : g.values(_) : [];
	},
	g.size = function (_) {
		return _.length === +_.length ? _.length : g.keys(_).length;
	},
	g.first = g.head = g.take = function (B, _, A) {
		return _ != null && !A ? M.call(B, 0, _) : B[0];
	},
	g.initial = function (B, _, A) {
		return M.call(B, 0, B.length - (_ == null || A ? 1 : _));
	},
	g.last = function (B, _, A) {
		return _ != null && !A ? M.call(B, Math.max(B.length - _, 0)) : B[B.length - 1];
	},
	g.rest = g.tail = g.drop = function (B, _, A) {
		return M.call(B, _ == null || A ? 1 : _);
	},
	g.compact = function (_) {
		return g.filter(_, function (_) {
			return !!_;
		});
	};
	var J = function (B, _, A) {
		return P(B, function (B) {
			g.isArray(B) ? _ ? R.apply(A, B) : J(B, _, A) : A.push(B);
		}),
		A;
	};
	g.flatten = function (A, _) {
		return J(A, _, []);
	},
	g.without = function (_) {
		return g.difference(_, M.call(arguments, 1));
	},
	g.uniq = g.unique = function (F, C, D, B) {
		var A = D ? g.map(F, D, B) : F,
		E = [],
		_ = [];
		return P(A, function (B, A) {
			if (C ? !A || _[_.length - 1] !== B : !g.contains(_, B)) {
				_.push(B),
				E.push(F[A]);
			}
		}),
		E;
	},
	g.union = function () {
		return g.uniq(K.apply(W, arguments));
	},
	g.intersection = function (A) {
		var _ = M.call(arguments, 1);
		return g.filter(g.uniq(A), function (A) {
			return g.every(_, function (_) {
				return g.indexOf(_, A) >= 0;
			});
		});
	},
	g.difference = function (A) {
		var _ = K.apply(W, M.call(arguments, 1));
		return g.filter(A, function (A) {
			return !g.contains(_, A);
		});
	},
	g.zip = function () {
		var C = M.call(arguments),
		A = g.max(g.pluck(C, "length")),
		B = new Array(A);
		for (var _ = 0; _ < A; _++) {
			B[_] = g.pluck(C, "" + _);
		}
		return B;
	},
	g.object = function (D, B) {
		var C = {};
		for (var A = 0, _ = D.length; A < _; A++) {
			B ? C[D[A]] = B[A] : C[D[A][0]] = D[A][1];
		}
		return C;
	},
	g.indexOf = function (D, B, C) {
		if (D == null) {
			return -1;
		}
		var A = 0,
		_ = D.length;
		if (C) {
			if (typeof C != "number") {
				return A = g.sortedIndex(D, B),
				D[A] === B ? A : -1;
			}
			A = C < 0 ? Math.max(0, _ + C) : C;
		}
		if (c && D.indexOf === c) {
			return D.indexOf(B, C);
		}
		for (; A < _; A++) {
			if (D[A] === B) {
				return A;
			}
		}
		return -1;
	},
	g.lastIndexOf = function (D, B, C) {
		if (D == null) {
			return -1;
		}
		var A = C != null;
		if (V && D.lastIndexOf === V) {
			return A ? D.lastIndexOf(B, C) : D.lastIndexOf(B);
		}
		var _ = A ? C : D.length;
		while (_--) {
			if (D[_] === B) {
				return _;
			}
		}
		return -1;
	},
	g.range = function (E, B, C) {
		arguments.length <= 1 && (B = E || 0, E = 0),
		C = arguments[2] || 1;
		var A = Math.max(Math.ceil((B - E) / C), 0),
		_ = 0,
		D = new Array(A);
		while (_ < A) {
			D[_++] = E,
			E += C;
		}
		return D;
	};
	var _ = function () {};
	g.bind = function (C, D) {
		var B,
		A;
		if (C.bind === j && j) {
			return j.apply(C, M.call(arguments, 1));
		}
		if (!g.isFunction(C)) {
			throw new TypeError;
		}
		return A = M.call(arguments, 2),
		B = function () {
			if (this instanceof B) {
				_.prototype = C.prototype;
				var F = new _,
				E = C.apply(F, A.concat(M.call(arguments)));
				return Object(E) === E ? E : F;
			}
			return C.apply(D, A.concat(M.call(arguments)));
		};
	},
	g.bindAll = function (A) {
		var _ = M.call(arguments, 1);
		return _.length == 0 && (_ = g.functions(A)),
		P(_, function (_) {
			A[_] = g.bind(A[_], A);
		}),
		A;
	},
	g.memoize = function (B, _) {
		var A = {};
		return _ || (_ = g.identity),
		function () {
			var C = _.apply(this, arguments);
			return g.has(A, C) ? A[C] : A[C] = B.apply(this, arguments);
		};
	},
	g.delay = function (B, _) {
		var A = M.call(arguments, 2);
		return setTimeout(function () {
			return B.apply(null, A);
		}, _);
	},
	g.defer = function (_) {
		return g.delay.apply(g, [_, 1].concat(M.call(arguments, 1)));
	},
	g.throttle = function (H, E) {
		var F,
		C,
		A,
		G,
		_,
		D,
		B = g.debounce(function () {
				_ = G = !1;
			}, E);
		return function () {
			F = this,
			C = arguments;
			var I = function () {
				A = null,
				_ && (D = H.apply(F, C)),
				B();
			};
			return A || (A = setTimeout(I, E)),
			G ? _ = !0 : (G = !0, D = H.apply(F, C)),
			B(),
			D;
		};
	},
	g.debounce = function (D, B, C) {
		var A,
		_;
		return function () {
			var H = this,
			E = arguments,
			G = function () {
				A = null,
				C || (_ = D.apply(H, E));
			},
			F = C && !A;
			return clearTimeout(A),
			A = setTimeout(G, B),
			F && (_ = D.apply(H, E)),
			_;
		};
	},
	g.once = function (B) {
		var _ = !1,
		A;
		return function () {
			return _ ? A : (_ = !0, A = B.apply(this, arguments), B = null, A);
		};
	},
	g.wrap = function (A, _) {
		return function () {
			var B = [A];
			return R.apply(B, arguments),
			_.apply(this, B);
		};
	},
	g.compose = function () {
		var _ = arguments;
		return function () {
			var A = arguments;
			for (var B = _.length - 1; B >= 0; B--) {
				A = [_[B].apply(this, A)];
			}
			return A[0];
		};
	},
	g.after = function (A, _) {
		return A <= 0 ? _() : function () {
			if (--A < 1) {
				return _.apply(this, arguments);
			}
		};
	},
	g.keys = T || function (B) {
		if (B !== Object(B)) {
			throw new TypeError("Invalid object");
		}
		var _ = [];
		for (var A in B) {
			g.has(B, A) && (_[_.length] = A);
		}
		return _;
	},
	g.values = function (B) {
		var _ = [];
		for (var A in B) {
			g.has(B, A) && _.push(B[A]);
		}
		return _;
	},
	g.pairs = function (B) {
		var _ = [];
		for (var A in B) {
			g.has(B, A) && _.push([A, B[A]]);
		}
		return _;
	},
	g.invert = function (B) {
		var _ = {};
		for (var A in B) {
			g.has(B, A) && (_[B[A]] = A);
		}
		return _;
	},
	g.functions = g.methods = function (B) {
		var _ = [];
		for (var A in B) {
			g.isFunction(B[A]) && _.push(A);
		}
		return _.sort();
	},
	g.extend = function (_) {
		return P(M.call(arguments, 1), function (A) {
			for (var B in A) {
				_[B] = A[B];
			}
		}),
		_;
	},
	g.pick = function (B) {
		var _ = {},
		A = K.apply(W, M.call(arguments, 1));
		return P(A, function (A) {
			A in B && (_[A] = B[A]);
		}),
		_;
	},
	g.omit = function (C) {
		var A = {},
		B = K.apply(W, M.call(arguments, 1));
		for (var _ in C) {
			g.contains(B, _) || (A[_] = C[_]);
		}
		return A;
	},
	g.defaults = function (_) {
		return P(M.call(arguments, 1), function (A) {
			for (var B in A) {
				_[B] == null && (_[B] = A[B]);
			}
		}),
		_;
	},
	g.clone = function (_) {
		return g.isObject(_) ? g.isArray(_) ? _.slice() : g.extend({}, _) : _;
	},
	g.tap = function (A, _) {
		return _(A),
		A;
	};
	var d = function (J, G, H, C) {
		if (J === G) {
			return J !== 0 || 1 / J == 1 / G;
		}
		if (J == null || G == null) {
			return J === G;
		}
		J instanceof g && (J = J._wrapped),
		G instanceof g && (G = G._wrapped);
		var A = Z.call(J);
		if (A != Z.call(G)) {
			return !1;
		}
		switch (A) {
		case "[object String]":
			return J == String(G);
		case "[object Number]":
			return J != +J ? G != +G : J == 0 ? 1 / J == 1 / G : J == +G;
		case "[object Date]":
		case "[object Boolean]":
			return +J == +G;
		case "[object RegExp]":
			return J.source == G.source && J.global == G.global && J.multiline == G.multiline && J.ignoreCase == G.ignoreCase;
		}
		if (typeof J != "object" || typeof G != "object") {
			return !1;
		}
		var I = H.length;
		while (I--) {
			if (H[I] == J) {
				return C[I] == G;
			}
		}
		H.push(J),
		C.push(G);
		var _ = 0,
		F = !0;
		if (A == "[object Array]") {
			_ = J.length,
			F = _ == G.length;
			if (F) {
				while (_--) {
					if (!(F = d(J[_], G[_], H, C))) {
						break;
					}
				}
			}
		} else {
			var B = J.constructor,
			E = G.constructor;
			if (B !== E && !(g.isFunction(B) && B instanceof B && g.isFunction(E) && E instanceof E)) {
				return !1;
			}
			for (var D in J) {
				if (g.has(J, D)) {
					_++;
					if (!(F = g.has(G, D) && d(J[D], G[D], H, C))) {
						break;
					}
				}
			}
			if (F) {
				for (D in G) {
					if (g.has(G, D) && !(_--)) {
						break;
					}
				}
				F = !_;
			}
		}
		return H.pop(),
		C.pop(),
		F;
	};
	g.isEqual = function (A, _) {
		return d(A, _, [], []);
	},
	g.isEmpty = function (A) {
		if (A == null) {
			return !0;
		}
		if (g.isArray(A) || g.isString(A)) {
			return A.length === 0;
		}
		for (var _ in A) {
			if (g.has(A, _)) {
				return !1;
			}
		}
		return !0;
	},
	g.isElement = function (_) {
		return !!_ && _.nodeType === 1;
	},
	g.isArray = l || function (_) {
		return Z.call(_) == "[object Array]";
	},
	g.isObject = function (_) {
		return _ === Object(_);
	},
	P(["Arguments", "Function", "String", "Number", "Date", "RegExp"], function (_) {
		g["is" + _] = function (A) {
			return Z.call(A) == "[object " + _ + "]";
		};
	}),
	g.isArguments(arguments) || (g.isArguments = function (_) {
		return !!_ && !!g.has(_, "callee");
	}),
	typeof / . /  != "function" && (g.isFunction = function (_) {
		return typeof _ == "function";
	}),
	g.isFinite = function (_) {
		return g.isNumber(_) && isFinite(_);
	},
	g.isNaN = function (_) {
		return g.isNumber(_) && _ != +_;
	},
	g.isBoolean = function (_) {
		return _ === !0 || _ === !1 || Z.call(_) == "[object Boolean]";
	},
	g.isNull = function (_) {
		return _ === null;
	},
	g.isUndefined = function (_) {
		return _ === void 0;
	},
	g.has = function (A, _) {
		return B.call(A, _);
	},
	g.noConflict = function () {
		return k._ = N,
		this;
	},
	g.identity = function (_) {
		return _;
	},
	g.times = function (C, A, B) {
		for (var _ = 0; _ < C; _++) {
			A.call(B, _);
		}
	},
	g.random = function (A, _) {
		return _ == null && (_ = A, A = 0),
		A + (0 | Math.random() * (_ - A + 1));
	};
	var E = {
		escape : {
			"&" : "&amp;",
			"<" : "&lt;",
			">" : "&gt;",
			'"' : "&quot;",
			"'" : "&#x27;",
			"/" : "&#x2F;"
		}
	};
	E.unescape = g.invert(E.escape);
	var U = {
		escape : new RegExp("[" + g.keys(E.escape).join("") + "]", "g"),
		unescape : new RegExp("(" + g.keys(E.unescape).join("|") + ")", "g")
	};
	g.each(["escape", "unescape"], function (_) {
		g[_] = function (A) {
			return A == null ? "" : ("" + A).replace(U[_], function (A) {
				return E[_][A];
			});
		};
	}),
	g.result = function (B, _) {
		if (B == null) {
			return null;
		}
		var A = B[_];
		return g.isFunction(A) ? A.call(B) : A;
	},
	g.mixin = function (_) {
		P(g.functions(_), function (A) {
			var B = g[A] = _[A];
			g.prototype[A] = function () {
				var _ = [this._wrapped];
				return R.apply(_, arguments),
				L.call(this, B.apply(g, _));
			};
		});
	};
	var G = 0;
	g.uniqueId = function (A) {
		var _ = G++;
		return A ? A + _ : _;
	},
	g.templateSettings = {
		evaluate : /<%([\s\S]+?)%>/g,
		interpolate : /<%=([\s\S]+?)%>/g,
		escape : /<%-([\s\S]+?)%>/g
	};
	var D = /(.)^/,
	Y = {
		"'" : "'",
		"\\" : "\\",
		"\r" : "r",
		"\n" : "n",
		"\t" : "t",
		"\u2028" : "u2028",
		"\u2029" : "u2029"
	},
	O = /\\|'|\r|\n|\t|\u2028|\u2029/g;
	g.template = function (I, F, G) {
		G = g.defaults({}, G, g.templateSettings);
		var C = new RegExp([(G.escape || D).source, (G.interpolate || D).source, (G.evaluate || D).source].join("|") + "|$", "g"),
		A = 0,
		H = "__p+='";
		I.replace(C, function (D, E, B, _, C) {
			H += I.slice(A, C).replace(O, function (_) {
				return "\\" + Y[_];
			}),
			H += E ? "'+\n((__t=(" + E + "))==null?'':_.escape(__t))+\n'" : B ? "'+\n((__t=(" + B + "))==null?'':__t)+\n'" : _ ? "';\n" + _ + "\n__p+='" : "",
			A = C + D.length;
		}),
		H += "';\n",
		G.variable || (H = "with(obj||{}){\n" + H + "}\n"),
		H = "var __t,__p='',__j=Array.prototype.join,print=function(){__p+=__j.call(arguments,'');};\n" + H + "return __p;\n";
		try {
			var _ = new Function(G.variable || "obj", "_", H);
		} catch (E) {
			throw E.source = H,
			E;
		}
		if (F) {
			return _(F, g);
		}
		var B = function (A) {
			return _.call(this, A, g);
		};
		return B.source = "function(" + (G.variable || "obj") + "){\n" + H + "}",
		B;
	},
	g.chain = function (_) {
		return g(_).chain();
	};
	var L = function (_) {
		return this._chain ? g(_).chain() : _;
	};
	g.mixin(g),
	P(["pop", "push", "reverse", "shift", "sort", "splice", "unshift"], function (A) {
		var _ = W[A];
		g.prototype[A] = function () {
			var B = this._wrapped;
			return _.apply(B, arguments),
			(A == "shift" || A == "splice") && B.length === 0 && delete B[0],
			L.call(this, B);
		};
	}),
	P(["concat", "join", "slice"], function (A) {
		var _ = W[A];
		g.prototype[A] = function () {
			return L.call(this, _.apply(this._wrapped, arguments));
		};
	}),
	g.extend(g.prototype, {
		chain : function () {
			return this._chain = !0,
			this;
		},
		value : function () {
			return this._wrapped;
		}
	});
}).call(this);
$(function (B) {
	B("#signout").click(function (A) {
		var _ = disk.RuntimeEnv.getURLComponent(disk.RuntimeEnv.URL_PRIMARY);
		window.location.href = "https://passport.baidu.com/?logout&u=" + _;
		A.preventDefault();
	});
	B("#flag-2-signout").click(function (A) {
		var _ = encodeURIComponent(location.href);
		window.location.href = "https://passport.baidu.com/?logout&u=" + _;
		A.preventDefault();
	});
	var A = {
		disk : 1,
		tongxunlu : 2,
		xiangce : 3,
		note : 4,
		duanxin : 5,
		zhaohui : 6,
		youxi : 7,
		jiankang : 8,
		wenzhang : 40
	};
	B("#vip-identity").bind("mouseover", function (_) {
		_.stopPropagation();
	});
	B("#header").delegate(".has-pulldown", "mouseover", function (_) {
		B(this).find(".pulldown").show();
		_.stopPropagation();
	}).delegate(".has-pulldown", "mouseout", function (_) {
		B(this).find(".pulldown").hide();
		_.stopPropagation();
	}).find(".navs").delegate("a", "click", function (J) {
		var H = B(this),
		F = -1;
		if (H.hasClass("def-nav")) {
			if (H.attr("href").indexOf("/apps") >= 0) {
				F = 9;
			} else {
				if (!H.hasClass("pulldown-nav")) {
					F = 0;
				}
			}
		} else {
			var G = H.attr("class").split(/\s+/),
			C;
			for (var D = 0, _ = G.length; D < _; D++) {
				C = A[G[D]];
				if (C >= 1) {
					F = C;
					break;
				}
			}
		}
		if (disk.DEBUG) {
			console.log(F);
		}
		var E = function () {
			FileUtils._mDiskLog.send({
				type : "header_new_module",
				val : F
			});
		},
		I = function () {
			if (H.attr("target") == "_blank") {
				E();
				window.open(H.attr("href"));
			} else {
				B(window).bind("unload", function () {
					E();
				});
				window.location.href = H.attr("href");
			}
		};
		if (F != -1) {
			I();
			J.preventDefault();
			J.stopPropagation();
			return false;
		}
	});
	B("#header").delegate(".has-pulldown-special", "mouseover", function (_) {
		B(".pulldown-nav").css("background-color", "#F6F6F9");
		B(this).find(".pulldown").show();
		B(".pulldown-nav").css("color", "#333");
		B(".hd-main .navs .pulldown-nav em").css({
			"background-position" : "-89px -55px"
		});
		_.stopPropagation();
	}).delegate(".has-pulldown-special", "mouseout", function (_) {
		B(".pulldown-nav").css("background-color", "#0f0f0f");
		B(this).find(".pulldown").hide();
		B(".pulldown-nav").css("color", "#fff");
		B(".hd-main .navs .pulldown-nav em").css({
			"background-position" : "-89px -45px"
		});
		_.stopPropagation();
	});
	var D = disk.getParam("frm") == "hao123" ? true : false,
	_ = function (A, _) {
		var C = 0;
		if (A) {
			C = _.innerWidth() + B(".logo-yun", "#header").innerWidth();
		} else {
			C = B(".logo", "#header").innerWidth();
		}
		return C + B(".navs", "#header").innerWidth() + B(".info>ul", "#header").innerWidth() + 30 - B("#header").width();
	},
	C = function () {
		var _ = B("#header .navs").offset().left;
		B(".newhand-tips-dialog").css("left", _ + 30);
		B(".newhandTips3-bg").css("left", _ + 20);
	};
	B(window).bind("resize", function () {
		var A = B(".logo-hao123", "#header"),
		E = (D && (A.length > 0)),
		F = function () {
			B("#departForAd").addClass("h-hide");
			if (E) {
				if (_(E, A) > 0) {
					A.hide().next().css("marginLeft", 10);
				} else {
					A.show().next().css("marginLeft", 0);
				}
			}
			var D = B(".newhand-tips-dialog");
			if (D.length && D.css("display") == "block") {
				C();
			}
		};
		if (_(E, A) > 0) {
			F();
		} else {
			if (B("#departForAd").text().trim().length) {
				B("#departForAd").removeClass("h-hide");
			}
			if (_(E, A) > 0) {
				F();
			} else {
				A.show().next().css("marginLeft", 0);
			}
		}
	});
});
function getDomainUrl() {
	var _,
	A = window.location.host;
	if (A.indexOf("/") != -1) {
		urlArray = A.split("/");
		_ = urlArray[0];
	} else {
		_ = A;
	}
	_ = window.location.protocol + "//" + _;
	return _;
}
(function (M) {
	var L = M,
	J = [],
	T = {},
	X = {},
	P = {},
	W = "1.0.103",
	O = null,
	C = 0,
	S = null,
	A = 10 * 1000,
	B = 50,
	H = 10,
	G = typeof opera !== "undefined" && opera.toString() === "[object Opera]",
	D = "__inline__dummy_name__",
	Z = /(loaded|complete)/gi;
	if (typeof L.ark !== "undefined") {
		throw new Error("the ark package was shadowed by global variable of ark");
		return;
	}
	var K = function (_) {
		return (typeof _ == "object" && typeof _.length != "undefined" && typeof _.splice == "function");
	},
	R = function (_) {
		return typeof _ == "object" && _ != null && !("splice" in _);
	},
	E = function (C) {
		if (K(C)) {
			var _ = C.length,
			H = 0,
			A = null,
			D = 0,
			B = 0;
			while (H < _) {
				A = C[H];
				B = 1;
				if (K(A)) {
					D = A.length;
					A.unshift(H, 1);
					Array.prototype.splice.apply(C, A);
					if (D == 0) {
						B = 0;
					}
					_ = C.length;
					A = C[H];
				}
				if (R(A) && K(A.url)) {
					var J = A.url,
					F = null,
					I = null;
					for (var L = 0, G = J.length; L < G; L++) {
						F = J[L];
						if (typeof F !== "string") {
							throw new Error("not supporting embed declaration of sophiscated inline url array");
						}
						I = {};
						for (var E in A) {
							if (A.hasOwnProperty(E) && E != "url") {
								I[E] = A[E];
							}
						}
						I.url = F;
						J[L] = I;
					}
					if (J.length == 0) {
						B = 0;
					}
					J.unshift(H, 1);
					Array.prototype.splice.apply(C, J);
					_ = C.length;
				}
				H += B;
			}
		}
		return C;
	},
	U = (function () {
		var A = navigator.userAgent,
		_ = {};
		if (G) {
			_.opera = 1;
			if (/Opera\/([\d\.]+)/gi.test(A)) {
				_.opera = parseFloat(RegExp.$1);
			}
		} else {
			if (/AppleWebKit\/([\d\.]+)/gi.test(A)) {
				_.webkit = parseFloat(RegExp.$1);
				if (/Chrome\/([\d\.]+)/gi.test(A)) {
					_.chrome = parseFloat(RegExp.$1);
				}
			} else {
				if (/MSIE ([\d\.]+)/gi.test(A)) {
					_.ie = parseFloat(RegExp.$1);
				} else {
					if (/Gecko\/[^\s]*/gi.test(A)) {
						_.gecko = 1;
						if (/rv:([^\s\)]*)/.test(A)) {
							_.gecko = parseFloat(RegExp.$1);
						}
					}
				}
			}
		}
		return _;
	})();
	L.ark = {
		DEBUG : false,
		GLOBAL_CALLBACK : null,
		setDebugEnabled : function (_) {
			this.DEBUG = _;
		},
		id : function () {
			return ++C;
		},
		conf : function (_) {
			P = _ || {};
			if (typeof P.view != "undefined") {
				var A = L.ark;
				delete L.ark;
				L = P.view;
				L.ark = A;
			}
		},
		getLooper : function () {
			return S;
		},
		hasError : function () {
			var _ = arguments.callee.caller.arguments.length;
			O.log("testing callee argument length " + _);
			return _ == 0;
		},
		log : (typeof console != "undefined") ? function (_) {
			if (this.DEBUG) {
				console.log("[ArkDebugLog] ", _);
			}
		}
		 : function (_) {},
		error : (typeof console != "undefined") ? function (_) {
			if (this.DEBUG) {
				console.error("[ArkDebugLog] ", _);
			}
		}
		 : function (_) {},
		debug : function () {
			O.log("queue:" + J);
			O.log("_cache: " + T);
			for (var A in T) {
				if (T.hasOwnProperty(A)) {
					O.log("_cache entity=>" + A + ":" + T[A]);
				}
			}
			for (var _ in U) {
				if (U.hasOwnProperty(_)) {
					O.log("env " + _ + ":" + U[_]);
				}
			}
			O.log("has css load feature=>" + O.CSSLoader.hasCSSLoadFeature());
			O.log("exports bridge: " + P.exports);
		},
		packageCached : function (_) {
			return this.cached(O.MessageLooper.toCacheKey(_));
		},
		cached : function (_) {
			return typeof T[_] != "undefined";
		},
		cache : function (_, A) {
			O.log("cache " + _ + " with value " + A);
			T[_] = A;
		},
		assert : function (_, A) {
			if (typeof _ == "boolean" && !_) {
				this.log(A);
				throw new Error("assert failure:" + A);
			}
		},
		exports : function (C, D, E) {
			if (P.exports) {
				var B = C.split(".");
				if (B[0] != P.exports) {
					B.unshift(P.exports);
				}
				B.pop();
				var A = L,
				_ = null;
				while (B.length > 0) {
					_ = B.shift();
					if (!A[_]) {
						A[_] = {};
					}
					A = A[_];
				}
				A[D] = E;
			}
		}
	};
	O = L.ark;
	O.each = (typeof Array.prototype.forEach != "undefined") ? function (_, A) {
		Array.prototype.forEach.call(_, A);
	}
	 : function (B, C) {
		if (typeof item == "undefined" || typeof item.length == "undefined" || typeof C != "function") {
			throw new Error("argument error");
		}
		for (var A = 0, _ = B.length; A < _; A++) {
			C.call(B, B[A], A);
		}
	};
	var I = function () {
		this._mPrivateFlags = 0;
		this._mCallback = null;
		this._mIOConf = null;
		this._mNode = null;
	};
	I._sInstance = null;
	I.SUCCESS = 0;
	I.FAILURE = 1;
	I.LOADING = 2;
	I.getInstance = function () {
		return I._sInstance ? I._sInstance : (I._sInstance = new I());
	};
	I.prototype = {
		constuctor : I,
		load : function (C, _) {
			this._mCallback = _;
			this._mIOConf = C;
			var D = this._build(),
			A = false;
			if (document) {
				var B = document.getElementsByTagName("head");
				if (B) {
					B[0].appendChild(D);
					A = true;
				}
			}
			if (!A) {
				O.log("insert failed, document was not ready");
				if (typeof _ == "function") {
					_.call(this, I.FAILURE);
				}
			}
			this._mNode = D;
		},
		_remove0 : function () {
			if (this._mNode) {
				var _ = this._mNode.getAttribute("_install");
				if (_ === "0") {
					this._mNode.detachEvent("onreadystatechange", this._mInnerSucc);
					this._mNode.detachEvent("onerror", this._mInnerFail);
				} else {
					if (_ === "1") {
						this._mNode.removeEventListener("load", this._mInnerSucc);
						this._mNode.removeEventListener("error", this._mInnerFail);
					}
				}
				this._mNode = null;
			}
		},
		_handleLoad : function (_) {
			Z.lastIndex = 0;
			O.log("load complete " + this._mIOConf.url);
			if (_.type == "load" || Z.test((_.currentTarget || _.srcElement).readyState)) {
				O.log("load complete with url " + this._mIOConf.url);
				if (typeof this._mCallback == "function") {
					this._mCallback.call(this, I.SUCCESS);
				}
				this._remove0();
			}
		},
		_handleError : function (_) {
			O.log("load failure " + this._mIOConf.url);
			if (typeof this._mCallback == "function") {
				this._mCallback.call(this, I.FAILURE);
			}
			this._remove0();
		},
		_build : function () {
			var _ = P.xhtml ? document.createElementNS("http://www.w3.org/1999/xhtml", "html:script") : document.createElement("script");
			_.type = this._mIOConf.scriptType || P.scriptType || "text/javascript";
			_.charset = this._mIOConf.charset || P.charset || "utf-8";
			_.async = true;
			var D = this,
			A = function (_) {
				O.log(arguments, "<===succ");
				D._handleLoad(_);
			},
			C = function (_) {
				O.log(arguments, "<===error");
				D._handleError(_);
			};
			if (!this._mInnerSucc) {
				this._mInnerSucc = A;
				this._mInnerFail = C;
			}
			if (_.attachEvent && !(_.attachEvent.toString && _.attachEvent.toString().indexOf("[native code") < 0) && !G) {
				_.attachEvent("onreadystatechange", A);
				_.attachEvent("onerror", C);
				_.setAttribute("_install", "0");
			} else {
				_.addEventListener("load", A);
				_.addEventListener("error", C);
				_.setAttribute("_install", "1");
			}
			var B = O.MessageLooper.normalize(this._mIOConf.url, this._mIOConf.timestamp || P.timestamp || Math.random());
			_.src = B;
			return _;
		}
	};
	O.ScriptLoader = I;
	var a = function () {
		this._mPrivateFlags = 0;
		this._mCallback = null;
		this._mIOConf = null;
		this._mNode = null;
		this._mHasCSSLoad = O.CSSLoader.hasCSSLoadFeature();
		this._mPollingTimer = null;
		this._mPollingStart = 0;
		this._mStylesheets = 0;
	};
	a._sInstance = null;
	a.SUCCESS = 0;
	a.FAILURE = 1;
	a.LOADING = 2;
	a.LOADING = 1;
	a.getInstance = function () {
		return a._sInstance ? a._sInstance : (a._sInstance = new a());
	};
	a.hasCSSLoadFeature = function () {
		if (U.ie) {
			return true;
		}
		if (U.chrome) {
			return U.chrome > 18;
		}
		return (U.opera > 0 || (U.gecko && U.gecko >= 9) || (U.webkit && U.webkit > 535.24));
	};
	a.prototype = {
		constructor : a,
		load : function (D, _) {
			this._mIOConf = D || {};
			this._mCallback = _;
			if (!this._mIOConf.url) {
				throw new Error("url required to load css");
			}
			var E = this._build(),
			B = false,
			A = a.LOADING;
			if ((this._mPrivateFlags & A) == A) {
				O.log("CSSLoader still working, may supporting parral loading in the coming future");
				return;
			}
			this._mPrivateFlags |= A;
			if (document) {
				var C = document.getElementsByTagName("head");
				if (C) {
					C[0].appendChild(E);
					B = true;
				}
			}
			if (!B) {
				O.log("insert failed, document was not ready");
				if (typeof _ == "function") {
					_.call(this, a.FAILURE);
				}
			}
			this._mNode = E;
		},
		_remove0 : function () {
			if (this._mNode) {
				var _ = this._mNode.getAttribute("_install");
				if (_ === "0") {
					this._mNode.detachEvent("onreadystatechange", this._mInnerSucc);
					this._mNode.detachEvent("onerror", this._mInnerFail);
				} else {
					if (_ === "1") {
						this._mNode.removeEventListener("load", this._mInnerSucc);
						this._mNode.removeEventListener("error", this._mInnerFail);
					}
				}
				this._mNode = null;
			}
		},
		_handleLoad : function (_) {
			var A = a.LOADING;
			this._mPrivateFlags &= ~A;
			Z.lastIndex = 0;
			if (_.type == "load" || Z.test((_.currentTarget || _.srcElement).readyState)) {
				O.log("load css complete " + this._mIOConf.url);
				if (typeof this._mCallback == "function") {
					this._mCallback.call(this, a.SUCCESS);
				}
				this._remove0();
			}
		},
		_handleError : function (_) {
			O.log("load css failure " + this._mIOConf.url);
			var A = a.LOADING;
			this._mPrivateFlags &= ~A;
			this._remove0();
			if (typeof this._mCallback == "function") {
				this._mCallback.call(this, a.FAILURE);
			}
		},
		_checkAvailable : function () {
			if (U.gecko) {
				var _ = this._mNode.sheet;
				try {
					return _ && _.cssRules && _.cssRules.length;
				} catch (A) {
					return false;
				}
			} else {
				O.log("check stylesheet length=>" + document.styleSheets.length + ":" + this._mStylesheets);
				return document.styleSheets.length > this._mStylesheets && this._mNode.sheet.cssRules.length > 0;
			}
		},
		_buildNode : function () {
			var _ = P.xhtml ? document.createElementNS("http://www.w3.org/1999/xhtml", "html:link") : document.createElement("link");
			_.rel = "stylesheet";
			_.type = "text/css";
			return _;
		},
		_stop0 : function () {
			clearInterval(this._mPollingTimer);
			this._mPollingTimer = null;
			this._mPollingStart = 0;
			this._mStylesheets = 0;
		},
		_polling : function () {
			O.log("polling for css " + this._mIOConf.url);
			var C = a.LOADING,
			B = (this._mPrivateFlags & C) == C;
			if (!B && this._mPollingTimer) {
				clearInterval(this._mPollingTimer);
				this._mPollingTimer = null;
			} else {
				if (this._checkAvailable()) {
					this._stop0();
					this._handleLoad({
						type : "load"
					});
				} else {
					var _ = new Date().getTime();
					if (_ - this._mPollingStart > (this._mIOConf.timeout || A)) {
						this._stop0();
						this._handleError();
					}
				}
			}
		},
		_build : function () {
			var F = this,
			C = function (_) {
				F._handleLoad(_);
			},
			E = function (_) {
				F._handleError(_);
			};
			if (!this._mInnerSucc) {
				this._mInnerSucc = C;
				this._mInnerFail = E;
			}
			var _ = this._mHasCSSLoad,
			D = O.MessageLooper.normalize(this._mIOConf.url, this._mIOConf.timestamp || P.timestamp || Math.random()),
			A = null;
			if (!_) {
				if (U.gecko) {
					A = P.xhtml ? document.createElementNS("http://www.w3.org/1999/xhtml", "html:style") : document.createElement("style");
					A.type = "text/css";
					A.innerHTML = (this._mIOConf.charset ? '@charset "' + this._mIOConf.charset + '";' : "") + '@import "' + this._mIOConf.url + '";';
				} else {
					A = this._buildNode();
					A.href = D;
				}
				this._mPollingTimer = setInterval(function () {
						F._polling();
					}, B);
				this._mPollingStart = new Date().getTime();
				this._mStylesheets = document.styleSheets.length;
			} else {
				A = this._buildNode();
				if (A.attachEvent && !(A.attachEvent.toString && A.attachEvent.toString().indexOf("[native code") < 0) && !G) {
					A.attachEvent("onreadystatechange", C);
					A.attachEvent("onerror", E);
					A.setAttribute("_install", "0");
				} else {
					A.addEventListener("load", C);
					A.addEventListener("error", E);
					A.setAttribute("_install", "1");
				}
				A.href = D;
			}
			return A;
		}
	};
	O.CSSLoader = a;
	var V = function () {
		this._mPrivateFlags = 0;
		this._mTransactions = [];
		this._mTransaction = null;
		this._mPollingTimer = null;
		this._mInterruptCounter = 0;
		this._mStart = 0;
	};
	V.PATTERN_PACK = /\./gi;
	V.PATTERN_CACHE_KEY = /[\.\/\\]+/gi;
	V.LOADING = 1;
	V.LOOPING = 2;
	V.FETAL = 4;
	V.POLLING_INTERVAL = 32;
	V.INTERRUPT_THRESHOLD = 10;
	V.MAX_RETRY = 1;
	V.TYPE_CSS = "css";
	V.TYPE_JS = "js";
	V.PATTERN_TYPE = /\.(js|css)$/gi;
	V.PATTERN_EXT = /\.\w+$/gi;
	V.SCHEMA = /^\w+:\/\//gi;
	V.PATTERN_PACKAGE = /^\w+(?:\.\w+)+$/gi;
	V.normalize = function (_, B) {
		if (typeof B == "undefined" || B == null) {
			return _;
		}
		var A = _.indexOf("?") != -1,
		C = _.substring(_.length - 1);
		if (C == "&") {
			return _ + "t=" + B;
		} else {
			return A ? _ + "&t=" + B : _ + "?t=" + B;
		}
	};
	V.translate = function (_) {
		return _.replace(V.PATTERN_PACK, "/");
	};
	V.toCacheKey = function (_) {
		return _.replace(V.PATTERN_CACHE_KEY, "_").toLowerCase();
	};
	V.PATTERN_TRIM = /^\s+|\s+$/gi;
	V.trim = function (_) {
		return _.replace(V.PATTERN_TRIM, "");
	};
	V.prototype = {
		constructor : V,
		scheduled : function (C, E, D) {
			var A = null;
			A = this._mTransaction;
			if (null != A && this._isItemMatch(C, E, D, A)) {
				return true;
			}
			for (var B = 0, _ = this._mTransactions.length; B < _; B++) {
				A = this._mTransactions[B];
				if (this._isItemMatch(C, E, D, A)) {
					return true;
				}
			}
			return false;
		},
		_isItemMatch : function (A, C, B, _) {
			if (A != D && _.getPack() != A) {
				return false;
			}
			if (this._isRequirementMatch(_, B)) {
				if (A == D && C.toString() == _.getFn().toString()) {
					return true;
				} else {
					if (A != D && _.getPack() == A) {
						return true;
					}
				}
			}
			return false;
		},
		_isRequirementMatch : function (A, F) {
			var E = A.getRequirements(),
			_ = E.length,
			C = null,
			D = null;
			if (_ != F.length) {
				return false;
			}
			if (_ == 0) {
				return true;
			}
			for (var B = 0; B < _; B++) {
				C = typeof E[B] === "string" ? E[B] : E[B].url;
				D = typeof F[B] === "string" ? F[B] : F.url;
				if (C != D) {
					return false;
				}
			}
			return true;
		},
		enqueue : function (A, C, B) {
			var _ = V.FETAL;
			if ((this._mPrivateFlags & _) == _ && O.DEBUG) {
				return;
			}
			if (this.scheduled(A, C, B)) {
				O.log("++++++++++++++++++++++++++++++++++++++++++++++");
				O.log(A + " scheduled already<" + B.join(", " + ">"));
				O.log("++++++++++++++++++++++++++++++++++++++++++++++");
				return;
			}
			if (this._mTransaction != null) {
				if (!this._mTransaction.loadedOver()) {
					O.log("push back " + this._mTransaction.getPack());
					J.unshift(this._mTransaction);
				}
			}
			this._mTransaction = null;
			J.unshift(new Y(A, C, B));
			this._start0();
		},
		_start0 : function () {
			var _ = V.LOOPING,
			A = this;
			if ((this._mPrivateFlags & _) != _) {
				O.log("message looper polling timer start...");
				this._mPollingTimer = setInterval(function () {
						A._loop();
					}, V.POLLING_INTERVAL);
				this._mInterruptCounter = 0;
				this._mPrivateFlags |= _;
				this._mStart = new Date().getTime();
			}
		},
		_stop0 : function () {
			clearInterval(this._mPollingTimer);
			this._mPollingTimer = null;
			this._mPrivateFlags &= ~V.LOOPING;
			this._mInterruptCounter = 0;
		},
		parseTypeof : function (B) {
			var _ = null,
			A = null;
			if (typeof B === "string") {
				_ = this.parseTypeFromUrl(B);
				A = {
					type : _,
					url : B
				};
			} else {
				if (typeof B.url == "undefined") {
					throw new Error("the arguments in {use} is required carrying url info");
				}
				if (B.type == V.TYPE_JS || B.type == V.TYPE_CSS) {
					_ = B.type;
				} else {
					_ = this.parseTypeFromUrl(B.url);
				}
				A = B;
				A.type = _;
			}
			A.bundle = A.url;
			if (typeof A.noDependency == "undefined") {
				A.noDependency = A.type == V.TYPE_CSS;
			}
			if (A.enforceUrl === true) {}
			else {
				this.normalizeUrl(A);
			}
			return A;
		},
		_quickNormalizeUrl : function (_) {
			if (typeof _ == "string") {
				return _;
			}
			_.bundle = _.url;
			return _;
		},
		normalizeUrl : function (C) {
			var F = C.url;
			if (F.indexOf("/") > -1) {
				V.PATTERN_EXT.lastIndex = 0;
				if (!V.PATTERN_EXT.test(F)) {
					F += "." + C.type;
				}
			} else {
				V.PATTERN_TYPE.lastIndex = 0;
				if (V.PATTERN_TYPE.test(F)) {
					var B = "." + RegExp.$1;
					F = F.replace(V.PATTERN_TYPE, function (_) {
							return "";
						});
					F = V.translate(F);
					F += B;
				} else {
					F = V.translate(F);
					F += "." + C.type;
				}
			}
			V.PATTERN_PACKAGE.lastIndex = 0;
			if (P.exports && V.PATTERN_PACKAGE.test(C.bundle) && F.indexOf(P.exports + "/") == 0) {
				F = F.substring(P.exports.length + 1);
			}
			V.SCHEMA.lastIndex = 0;
			var E = V.SCHEMA.test(F);
			if (E && C.type == V.TYPE_JS) {
				C.noDependency = true;
			}
			if (!E && typeof P.baseUrl != "undefined") {
				var _ = C.type == V.TYPE_JS ? P.baseUrl[0] : P.baseUrl[1],
				A = _.lastIndexOf("/"),
				D = F.indexOf("/");
				if (A == _.length - 1 && D == 0) {
					F = F.substring(1);
				} else {
					if (A != _.length - 1 && D != 0) {
						F = "/" + F;
					}
				}
				F = _ + F;
			}
			C.url = F;
		},
		parseTypeFromUrl : function (_) {
			_ = V.trim(_);
			V.PATTERN_TYPE.lastIndex = 0;
			if (V.PATTERN_TYPE.test(_)) {
				return RegExp.$1;
			} else {
				return V.TYPE_JS;
			}
		},
		_load : function (B) {
			var A = V.LOADING,
			C = this,
			_ = this.parseTypeof(B);
			this._mPrivateFlags |= A;
			if (_.type == V.TYPE_CSS) {
				if (typeof P.callback == "function") {
					P.callback.call(O, O.CSSLoader.LOADING);
				}
				O.CSSLoader.getInstance().load(_, function (D) {
					O.log("****CSS:loading status of url " + this._mIOConf.url + "=========" + (D == a.SUCCESS));
					C._mPrivateFlags &= ~A;
					if (typeof P.callback == "function") {
						P.callback.call(O, D);
					}
					if (D == a.SUCCESS) {
						var B = V.toCacheKey(_.bundle);
						O.cache(B, true);
					}
				});
			} else {
				if (typeof P.callback == "function") {
					P.callback.call(O, O.ScriptLoader.LOADING);
				}
				O.ScriptLoader.getInstance().load(_, function (D) {
					O.log("****JS:loading status of url " + this._mIOConf.url + "=========" + (D == I.SUCCESS));
					C._mPrivateFlags &= ~A;
					if (typeof P.callback == "function") {
						P.callback.call(O, D);
					}
					V.SCHEMA.lastIndex = 0;
					if (_.noDependency || V.SCHEMA.test(_.url)) {
						var B = V.toCacheKey(_.bundle);
						O.cache(B, true);
						O.log("encounter an no dependency script<<<<<<<<<<<<<<<<<<<<<<<<<<<");
					}
				});
			}
		},
		_loop : function () {
			var G = V.LOADING,
			A = null;
			if ((this._mPrivateFlags & G) == G) {
				O.log("still loading...");
				return;
			}
			O.log("looping now");
			if (this._mTransaction != null) {
				if (!this._mTransaction.hasRequirement()) {
					A = V.toCacheKey(this._mTransaction.getPack());
					if (!O.cached(A)) {
						var C = null;
						try {
							C = this._mTransaction.getFn().apply(O, this._mTransaction.getRequirements());
						} catch (I) {
							O.error("########################################################");
							O.error("the callback of module " + this._mTransaction.getPack() + " is supposed to have error");
							O.error("error: " + I.message);
							O.error("########################################################");
							if (O.DEBUG) {
								this._stop0();
								this._mPrivateFlags |= V.FETAL;
								throw I;
							}
							C = true;
						}
						if (A != D) {
							O.cache(A, C);
							O.log("start cache no dependency item " + A);
						}
					}
					this._mTransaction = null;
				} else {
					var H = null;
					while ((H = this._mTransaction.next()) != null) {
						A = V.toCacheKey(typeof H === "string" ? H : H.url);
						if (O.cached(A)) {
							this._quickNormalizeUrl(H);
							continue;
						}
						this._load(H);
						break;
					}
					if (H == null) {
						var B = [],
						F = null,
						L = false,
						K = this;
						O.log("######resolve requirements of " + this._mTransaction.getPack());
						O.each(this._mTransaction.getRequirements(), function (_, C) {
							A = V.toCacheKey(typeof _ === "string" ? _ : _.bundle);
							if (!O.cached(A)) {
								O.log("---------------------error in " + K._mTransaction.getPack());
								L = true;
							} else {
								F = T[A];
								B[C] = typeof F == "boolean" ? null : F;
							}
						});
						A = V.toCacheKey(this._mTransaction.getPack());
						if (!L) {
							C = null;
							try {
								C = this._mTransaction.getFn().apply(O, B);
							} catch (I) {
								O.error("########################################################");
								O.error("the callback of module " + this._mTransaction.getPack() + " is supposed to have error");
								O.error("error: " + I.message);
								O.error("########################################################");
								if (O.DEBUG) {
									this._stop0();
									this._mPrivateFlags |= V.FETAL;
									throw I;
								}
								C = true;
							}
							if (A != D) {
								O.cache(A, C);
								delete X[A];
							}
						}
						if (A == D && L) {
							O.log("=========================report root failure=================================");
							var E = X[this._mTransaction.getId()];
							if (typeof E == "undefined") {
								E = 0;
							} else {
								E++;
							}
							X[this._mTransaction.getId()] = E;
							if (E < V.MAX_RETRY - 1) {
								O.log("guys, retry to figure out dependency chain at " + E);
								this._mTransaction.loadOver();
							} else {
								delete X[this._mTransaction.getId()];
								try {
									this._mTransaction.getFn().apply(O);
								} catch (I) {
									O.error("########################################################");
									O.error("the top callback is supposed to have error");
									O.error("error: " + I.message);
									O.error("########################################################");
									if (O.DEBUG) {
										this._stop0();
										this._mPrivateFlags |= V.FETAL;
										throw I;
									}
								}
								this._mTransaction = null;
							}
						} else {
							this._mTransaction = null;
						}
					}
				}
			} else {
				if (J.length == 0) {
					this._mInterruptCounter++;
					if (this._mInterruptCounter > V.INTERRUPT_THRESHOLD) {
						O.log("looper idle, is about to shutting down");
						this._stop0();
						var _ = new Date().getTime();
						O.log("#########cost " + (_ - this._mStart) / 1000 + "s to complete the session, hm, you should minus " + V.POLLING_INTERVAL * V.INTERRUPT_THRESHOLD + "ms idle#########");
					}
				} else {
					this._mTransaction = J.shift();
					O.log("take=>" + this._mTransaction.getPack());
				}
			}
		}
	};
	O.MessageLooper = V;
	S = new V();
	O.parseTypeFromUrl = S.constructor.prototype.parseTypeFromUrl;
	O.parseTypeof = S.constructor.prototype.parseTypeof;
	O.normalizeUrl = S.constructor.prototype.normalizeUrl;
	var Y = function (_, B, A) {
		this._mPack = _;
		this._mFn = B;
		this._mReq = A;
		this._mCursor = -1;
		this._mId = O.id();
	};
	Y.prototype = {
		constuctor : Y,
		next : function () {
			if (this._mCursor < this._mReq.length) {
				this._mCursor++;
			}
			return this._mCursor > this._mReq.length - 1 ? null : this._mReq[this._mCursor];
		},
		getId : function () {
			return this._mId;
		},
		prev : function () {
			if (this._mCursor > -1) {
				this._mCursor--;
			}
			return this._mCursor < 0 ? null : this._mReq[this._mCursor];
		},
		hasRequirement : function () {
			return this._mReq.length > 0;
		},
		getFn : function () {
			return this._mFn;
		},
		getPack : function () {
			return this._mPack;
		},
		getRequirements : function () {
			return this._mReq;
		},
		loadedOver : function () {
			return this._mReq.length == 0 || this._mCursor > this._mReq.length - 1;
		},
		loadOver : function () {
			this._mCursor = -1;
		}
	};
	var F = function (C, H, G) {
		var A = null;
		if (G.length > 0) {
			var F = true,
			E = [];
			for (var B = 0, _ = G.length; B < _; B++) {
				A = O.MessageLooper.toCacheKey(typeof G[B] == "string" ? G[B] : G[B].url);
				if (!O.cached(A)) {
					F = false;
				} else {
					E.push(T[A]);
				}
			}
			if (C == D) {}
			else {
				A = O.MessageLooper.toCacheKey(C);
				F = F && O.cached(A);
			}
			if (F) {
				H.apply(O, E);
			}
			return F;
		} else {
			if (C == D) {
				H.call(O);
				return true;
			}
			A = O.MessageLooper.toCacheKey(C);
			if (!O.cached(A)) {
				return false;
			}
			H.call(O, typeof T[A] == "boolean" ? null : T[A]);
			return true;
		}
	};
	O.match = F;
	var N = function () {
		O.log("before add in queue " + arguments[0]);
		var A = arguments;
		if (A.length < 2) {
			throw new Error("require one param at least");
		}
		var B = [],
		C = A[A.length - 1],
		_ = A[0];
		if (typeof C != "function") {
			throw new Error("the trailing argument should be a callback function");
		}
		if (typeof _ != "string" || _.length == 0) {
			throw new Error("the leading argument should be a legal name");
		}
		if (A.length > 2) {
			B = Array.prototype.slice.call(A, 1, A.length - 1);
		}
		B = E(B);
		if (B.length > (P.maxInlineDependency || H)) {
			throw new Error("too many dependency request, " + "why would you think about combining some of them together to reduce amount");
		}
		if (O.match(_, C, B)) {
			O.log('{matching a cache for "' + _ + " request " + B.join(", ") + '"}');
			return;
		}
		S.enqueue(_, C, B);
	};
	O.add = N;
	var Q = function (A) {
		var _ = typeof A === "string" ? {
			url : A
		}
		 : A;
		_.noDependency = true;
		O.use.call(O.use, _, function () {});
	};
	O.addCacheHint = Q;
	var _ = function () {
		var _ = Array.prototype.slice.call(arguments, 0);
		N.apply(N, [D].concat(_));
	};
	O.use = _;
})(window);
if (typeof disk == "undefined") {
	var disk = {
		util : {},
		ui : {},
		api : {},
		tpl : {},
		widget : {},
		DEBUG : false
	};
	if (typeof console == "undefined") {
		var console = {
			log : function () {},
			assert : function () {},
			info : function () {},
			warn : function () {},
			error : function () {}

		};
	}
	$.extend({
		stringify : function stringify(_) {
			var A = typeof(_);
			if (A != "object" || _ === null) {
				if (A == "string") {
					_ = '"' + _ + '"';
				}
				return String(_);
			} else {
				var D,
				E,
				C = [],
				B = (_ && _.constructor == Array);
				for (D in _) {
					E = _[D];
					A = typeof(E);
					if (_.hasOwnProperty(D)) {
						if (A == "string") {
							E = '"' + E + '"';
						} else {
							if (A == "object" && E !== null) {
								E = $.stringify(E);
							}
						}
						C.push((B ? "" : '"' + D + '":') + String(E));
					}
				}
				return (B ? "[" : "{") + String(C) + (B ? "]" : "}");
			}
		},
		parseJSON : function (B) {
			var _ = /^[\],:{}\s]*$/,
			D = /\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g,
			A = /"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g,
			C = /(?:^|:|,)(?:\s*\[)+/g;
			if (typeof B === "object") {
				return B;
			}
			if (typeof B !== "string" || !B) {
				return null;
			}
			B = $.trim(B);
			if (_.test(B.replace(D, "@").replace(A, "]").replace(C, ""))) {
				return window.JSON && window.JSON.parse ? window.JSON.parse(B) : (new Function("return " + B))();
			} else {
				jQuery.error("Invalid JSON: " + B);
			}
		}
	});
	String.prototype.len = (function () {
		var _ = /[^\x00-\xFF]/g;
		return this.replace(_, "..").length;
	});
	if (typeof String.prototype.trim == "undefined") {
		String.prototype.trim = function () {
			return this.replace(/^\s+|\s+$/gi, "");
		};
	}
	String.prototype.toEntity = function () {
		return $("<div></div>").text(this.toString()).html();
	};
	disk.uniqueId = 0;
	disk.obtainId = function () {
		return "_disk_id_" + (++disk.uniqueId);
	};
	disk.iframeLogin = function () {
		var _ = function () {
			var _ = disk.ui.LoginDialog.obtain();
			disk.docreaderVisible(false);
			_.setVisible(true);
			_.onLoginSuccessCallback = function () {};
			_.onCloseCallback = function () {
				disk.docreaderVisible(true);
			};
		};
		if ($.browser.msie && $.browser.version == "6.0") {
			setTimeout(function () {
				_();
			}, 50);
		} else {
			_();
		}
	};
	disk.docreaderVisible = function (A) {
		if (!$("#chunleireader").length) {
			return;
		}
		var _ = disk.Context.getService(disk.Context.SERVICE_CANVAS);
		if (A) {
			$("#chunleireader").css("display", "block");
			if (!disk.util.ViewShareUtils) {
				_.setVisible(A);
			}
		} else {
			$("#chunleireader").css("display", "none");
			if (!disk.util.ViewShareUtils) {
				_.setVisible(A);
			}
		}
	};
	disk.getParam = function (C, B) {
		var A = new RegExp("(?:^|\\?|#|&)" + C + "=([^&#]*)(?:$|&|#)", "i"),
		_ = A.exec(B || location.href);
		return _ ? _[1] : "";
	};
	disk.setParam = function (C, D, B) {
		var A = new RegExp("(?:^|\\?|#|&)" + C + "=([^&#]*)(?:$|&|#)", "i"),
		_ = A.exec(B || location.href);
		if (_ != null) {
			return B.replace(_[1], D);
		}
		return null;
	};
	var RuntimeEnv = {
		getURLComponent : function (_) {
			if (_ == disk.RuntimeEnv.URL_PROTOCOL) {
				return location.protocol;
			} else {
				if (_ == disk.RuntimeEnv.URL_PORT) {
					return location.port;
				} else {
					if (_ == disk.RuntimeEnv.URL_HOST) {
						return location.host;
					} else {
						if (_ == disk.RuntimeEnv.URL_PRIMARY) {
							return location.protocol + "//" + location.host;
						} else {
							return null;
						}
					}
				}
			}
		}
	};
	RuntimeEnv.URL_PROTOCOL = 1;
	RuntimeEnv.URL_PORT = 2;
	RuntimeEnv.URL_HOST = 4;
	RuntimeEnv.URL_PRIMARY = 8;
	RuntimeEnv.URL = 16;
	disk.RuntimeEnv = RuntimeEnv;
}
try {
	if (window.console && window.console.log && top === self) {
		console.log("\n\n\n%c", 'font-size:0;margin-top:20px;line-height:36px;padding-top:50px;padding-left:158px;background:url("data:image/png;base64,/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRofHh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/wAARCAAyAJ4DASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD3+iiigCtfX9vp1sZ7hwqjoO5PoK5afxu/mH7ParszwXPNc94y1sz6/Nbb/wB1a/IF/wBrqTXR+HvCls+nxXWoqZZZlDiPJAQHp+NerDD0aFFVK2rZ5U69avVdOi7JBa+NgXAurXap/ijOcV1Vtcw3kCzwOHjYcEVxnijwzBZae9/YBkEXMkecjHqM+lUvAest/ar6ezZjmQso9GH/ANalVw1KrRdahpbdDpV61KsqVbW56NRRRXlnqBRRVOTVtOhlaKW/tkkU4ZWlUEH6ZoAuUVQ/tvSv+glaf9/l/wAaP7b0r/oJWn/f5f8AGgC/RUMF1b3MXmwTxyxg43owI/On+dF/z1T/AL6FAD6Ko2utaXfSyRWmo2s8kX31jlVivbnBq2JYycCRCT2DUAPopjTRKcNIgI7FhSefD/z1j/76FAElFR+fD/z1j/76FPVlYZVgR6g0AMmmjgiaWVgqKMkmuO1fxrIkEj6YqkoM/vE+8O+DmqfxF1loJrXTVbarKZXHrzgf1qPwh4ei1iye8vdzW7MUSMHG7HXJ9K9COEgsM61XrscDxlT61GlSSdt76+p59qN1/al5LeyuUmmYu5XkE/SvY/CPiG01bRrePz4xdwxqksecHIGMgehrP174f6XdafIdMgFrdopMe1jtYjsQf5145E00krBJHh8s/PIpIK89OO5rzqVXE1akKFR8yb062PqcRh8sqYWri8PHklFXfS/6a7K3U9u8Zarbpo9xp6XCC4uF2HHOxT1OPXHQVwWivaaHex3lusktwgIDSvxzweBWVpkV1q1/DZWiPJK/djnA7lj/AFr1aw8FaTa2qx3EP2mbHzyOSMn2APFfQS9jgqfspNu58NH22Nqe0jpbb+u5X03xzaXEqxXqfZ2bgODlfx9K6tWDKGUgg8gjvXlPjbw+mgPDdWhY2kzFdpOSjdcZ9MfyrovhzrL6hpc9pKxZrVwFJ67T0rjxOFpuj9Yo7HZhsTUVb2Fbc7WvIfGeneCrHxNctq39q/bLj9+/kBSnPp+VevV4x8Q7yPT/AIlWN7NEZY4FikeMYywDZI5rzD0zjteHh3fCNAW82YPmm7xnPbGPxrVtE8AfY4RdnWWudg80xqu0t3xz0rr/APha+g/9C7P/AN8x1heLfHel+IdDaws9HktZTIr+YyoAAO3HNAHaWdlpln8Kb59HNx9juLeWZPPPzgng5x9K8tMOm2ehaddXEN9PcXRcsVm2oqq2MD3r1DS/+SKD/sHyfzauDll05vAWhWd9eS2rmea4jZITICAdv55NAEKweGAMjSPEYJ9HH/xNXvCUmjnxzYRQWGpxssuYzLPkqdp++uOn/wBaro8a6hHEMeJbsIqjBOkDGPrip/BmqaXL47+2y6pdX2pXymMAWnlr0HJ/BewoA6jXfhpYa9rNxqU2oXcckxGUTGBgAentWd/wp3S/+gpff+O/4Vk/FLRo9PuG1ganP9ovZQkdoowMBQCc59h27irWn/Cqa5063nudduoZ5I1d4wmdhIzjrQBc/wCFO6X/ANBS+/8AHf8ACuz8O6FD4c0aLTIJpJo42Zg8mMncSf61xDfCVI1LN4kulUdSY8Af+PV2nhjR00LQobCO8a8VGZhM3U5Yn1PTOKAPPvi1aSw6lYagobyniMRI6Bgcj8wT+VXPhv4stI9OOk3kgieNy0bN0IPJ/XNd9rGkWmuaZLY3ibo3HBHVT2I968h1T4c6zps5MEbXUIOUlhOG/EdQa9vD1KOJwyw9R2a2PHrwq4bEPEU48ye56jrPifT9M02a4FzG7Kp2lTkA9s/4d68HjE+pX0dpYwu7SSHy4x1Zj1J9z+lbsXg7xFqUqxta3bY6G4YhV9/mr0vwf4Kt/DUZuJmWfUJBhpMcIPRf8aqCw+Ai535p9CZzr49qmk4w3f8AWl/Is+EPCsPhrTsNiS+mAM8v/so9hXR0UV4tSpKpNzm9WevTpxpxUIrRGJ4p8Px+JNHNm85gdXEkcgGQGAI5HcYJqn4O8Jx+GLacm6+0zzkFnC4UAdABU3jKPVJNDzpUZmkSQNJEpwXTByB+ODj2rO8AW2tRWt3LqkUlvFI4MMEh5HqfbNdkef6o/f0vsccuT62vc1tudlXjnxAmay+JFjfG1knjt1ikZFX7wDZI6V7HRXAd55h/ws+w/wChXn/Jf/iawvFnjKDxFoh0+20Ga2kMiv5m0Hp24Fe2UUAcPZWlxb/B77NNDIk4sHBjK/Nzkjj8a4Sy1Oxi8H2+i6r4cvLtgXkSZPkaMlj904yOle50UAfPsmn+M5PDoR7fUjpAbiIjkL2+Xrj3xiuv8D6rpen6ha6dZeGb2Ga4bZJez/M3TOSccDjoMda9TooA8a8fX00/j+3M+n3FzY6dsxGgIEnRjzjvwPwrc/4WvL/0LN5/38/+xr0migDyPxF8QZ9c0C801fD95C1wm0SFydvPXG2ux+HFtcWngixjuYXikJdtrgg4LEg8+orq6KACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigD//Z");background-repeat:no-repeat;');
		console.log("\u54a6\uff01\u8fd9\u4e48\u5de7\u4f60\u4e5f\u559c\u6b22\u7814\u7a76WEB\u6280\u672f\uff01\r\n\u53c8\u8fd9\u4e48\u5de7\u6211\u4eec\u6025\u9700\u50cf\u4f60\u8fd9\u6837\u7684\u4e13\u4e1aWEB\u4eba\u58eb!\r\n\u6765\u767e\u5ea6\u4e91\u5427\uff0c\u548c\u6211\u4eec\u4e00\u8d77\u6539\u53d8\u767e\u5ea6\u4e91\uff01\u6539\u53d8\u4e91\u5b58\u50a8\uff01\r\n\u804c\u4f4d\u4ecb\u7ecd\uff0c\u524d\u7aef\uff1ahttp://dwz.cn/mbufe \u540e\u7aef\uff1ahttp://dwz.cn/frvsI");
		console.log("\r\u8bf7\u5c06\u7b80\u5386\u53d1\u9001\u81f3 %c spacehr@baidu.com\uff08 \u90ae\u4ef6\u6807\u9898\u8bf7\u4ee5\u201c\u59d3\u540d-\u5e94\u8058XX\u804c\u4f4d-\u6765\u81eaconsole-FE/SERVER\u201d\u547d\u540d\uff09", "color:red");
	}
} catch (e) {}

(function () {
	var _ = function (_) {
		this._mUI = _;
		this._mPrivateFlags = 0;
		this._mCoor = [];
	};
	_.VISIBLE = 1;
	_.TOP = 4096;
	_.RIGHT = 8192;
	_.BOTTOM = 16384;
	_.LEFT = 32768;
	_.CENTER = 65536;
	_.prototype = {
		setVisible : function (A) {
			var _ = (this._mPrivateFlags & disk.ui.Panel.VISIBLE) == disk.ui.Panel.VISIBLE;
			if (A === _) {
				return;
			}
			if (A) {
				this._mPrivateFlags |= disk.ui.Panel.VISIBLE;
				this._mUI.pane.style.display = "block";
			} else {
				this._mPrivateFlags &= ~disk.ui.Panel.VISIBLE;
				this._mUI.pane.style.display = "none";
			}
			this.onVisibilityChange(A);
		},
		setVisibleAnimation : function (B, _) {
			var A = (this._mPrivateFlags & disk.ui.Panel.VISIBLE) == disk.ui.Panel.VISIBLE;
			if (B === A) {
				return;
			}
			if (B) {
				this._mPrivateFlags |= disk.ui.Panel.VISIBLE;
				$(this._mUI.pane).css({
					display : "block",
					opacity : 0
				}).animate({
					opacity : _
				}, 500);
			} else {
				this._mPrivateFlags &= ~disk.ui.Panel.VISIBLE;
				$(this._mUI.pane).hide().css("opacity", 0);
			}
			this.onVisibilityChange(B);
		},
		isVisible : function () {
			return (this._mPrivateFlags & disk.ui.Panel.VISIBLE) == disk.ui.Panel.VISIBLE;
		},
		setSize : function (_, A) {
			this._mUI.pane.style.width = _ + "px";
			this._mUI.pane.style.height = A + "px";
		},
		setLocation : function (A, _) {
			_ = Math.max(0, _);
			this._mUI.pane.style.left = A + "px";
			this._mUI.pane.style.top = _ + "px";
			this._mCoor[0] = A;
			this._mCoor[1] = _;
		},
		getLocation : function () {
			return this._mCoor;
		},
		setGravity : function (A) {
			switch (A) {
			case _.LEFT:
				this.setLocation(0, 0);
				break;
			case _.CENTER:
				this.setLocation(($(window).width() - this._mUI.pane.offsetWidth) / 2, ($(window).height() - this._mUI.pane.offsetHeight) / 2 + $(window).scrollTop());
				break;
			case _.TOP:
				this.setLocation(($(window).width() - this._mUI.pane.offsetWidth) / 2, ($(window).scrollTop() || 0) + 80);
				break;
			}
		},
		onVisibilityChange : function () {}

	};
	disk.ui.Panel = _;
})();
(function () {
	var _ = function (_) {
		disk.ui.Panel.call(this, this._prebuild());
		this._init();
		this._mCallReferCount = 0;
		this._mBlurCallback = null;
	};
	_.prototype = new disk.ui.Panel();
	_.prototype._prebuild = function () {
		var _ = document.createElement("div");
		_.className = "b-panel b-canvas";
		document.body.appendChild(_);
		return {
			pane : _
		};
	};
	_.prototype._checkCallReferCount = function (_) {
		if (_) {
			this._mCallReferCount++;
		} else {
			this._mCallReferCount--;
		}
		if (_ === true) {
			return true;
		} else {
			return this._mCallReferCount <= 0;
		}
	};
	_.prototype.setVisible = function (_) {
		if (!this._checkCallReferCount(_)) {
			return;
		}
		this.setGravity(disk.ui.Panel.LEFT);
		this._size();
		disk.ui.Panel.prototype.setVisible.call(this, _);
	};
	_.prototype.setVisibleAnimation = function (A, _) {
		if (!this._checkCallReferCount(A)) {
			return;
		}
		this.setGravity(disk.ui.Panel.LEFT);
		this._size();
		disk.ui.Panel.prototype.setVisibleAnimation.call(this, A, _);
	};
	_.prototype._size = function () {
		this.setSize(0, 0);
		var _ = (document.compatMode != "CSS1Compat" || $.browser.safari) ? document.body.scrollWidth : document.documentElement.scrollWidth,
		A = Math.max(_, $(window).width());
		this.setSize(_, $(document).height());
	};
	_.prototype.setOnBlurCallback = function (_) {
		this._mBlurCallback = _;
	};
	_.prototype._init = function () {
		var _ = this;
		$(window).bind("resize", function () {
			if (_.isVisible()) {
				_._size();
			}
		});
		this._mUI.pane.onclick = function () {
			if (_._mBlurCallback != null) {
				_._mBlurCallback.call(_);
			}
		};
	};
	disk.ui.Canvas = _;
})();
(function () {
	var _ = function () {
		this._mMode = 0;
		this._mMsgHolderId = disk.obtainId();
		this._mMsgHolder = null;
		disk.ui.Panel.call(this, this._prebuild());
		this._mTimerService = null;
		this._pendTask = null;
	};
	_.MODE_LOADING = 1;
	_.MODE_CAUTION = 2;
	_.MODE_SUCCESS = 4;
	_.MODE_FAILURE = 8;
	_.DURATION_DISAPPEAR = 3000;
	_.prototype = new disk.ui.Panel();
	_.prototype._prebuild = function () {
		var _ = document.createElement("div");
		_.className = "b-panel b-dialog toast-dialog toast-content b-bdr-6 bdr-rnd-3 box-shadow";
		document.body.appendChild(_);
		_.innerHTML = '<div class="toast-outer">' + '<div id="' + this._mMsgHolderId + '" class="toast-msg ellipsis">' + '<em class="b-in-blk b-ic-dimen-1 loading"></em>' + "</div>" + "</div>";
		return {
			pane : _
		};
	};
	_.prototype.pending = function (_) {
		this._pendTask = _;
	};
	_.prototype.setVisible = function (B, A) {
		var _ = 5000,
		C = this;
		if (this._mTimerService && this._mTimerService.isAlive()) {
			this._mTimerService.interrupt();
		}
		if (B) {
			this._mUI.pane.style.display = "block";
			this.setGravity(disk.ui.Panel.CENTER);
			C._scheduleHiding(A);
		} else {
			C._mUI.pane.style.display = "none";
			if (C._pendTask != null) {
				C._pendTask.call(C);
				C._pendTask = null;
			}
		}
	};
	_.prototype.setPositionRB = function (A, _) {
		A = $(window).width() - this._mUI.pane.offsetWidth - Math.max(32, A);
		_ = $(window).height() - this._mUI.pane.offsetHeight - Math.max(42, _);
		this._mUI.pane.style.left = A + "px";
		this._mUI.pane.style.top = _ + "px";
	};
	_.prototype._scheduleHiding = function (_) {
		var A = this;
		if (!this._mTimerService) {
			this._mTimerService = new disk.util.TimerService(disk.ui.Toast.DURATION_DISAPPEAR, function () {
					A.setVisible(false);
				});
		}
		if ("boolean" == typeof _ && !_) {
			this._mTimerService.start();
		} else {
			if ("number" == typeof _ && _ > 0) {
				this._mTimerService.setInterval(_);
				this._mTimerService.start();
			}
		}
	};
	_.prototype.setMessage = function (B, A) {
		if (!this._mMsgHolder) {
			this._mMsgHolder = document.getElementById(this._mMsgHolderId);
		}
		var _ = null;
		switch (B) {
		case disk.ui.Toast.MODE_LOADING:
			_ = "loading";
			break;
		case disk.ui.Toast.MODE_CAUTION:
			_ = "ic-mini-caution";
			break;
		case disk.ui.Toast.MODE_SUCCESS:
			_ = "ic-mini-ok";
			break;
		case disk.ui.Toast.MODE_FAILURE:
			_ = "ic-mini-failure";
			break;
		}
		if (_ == null) {
			throw new Error("not hit a toast type exception");
		}
		this._mMsgHolder.innerHTML = '<em class="sprite-ic b-in-blk b-ic-dimen-1 ' + _ + '"></em>' + A + '<em class="close-tips"> </em>';
	};
	_.prototype.setClose = function (A) {
		var B = this,
		_ = $("#" + this._mMsgHolderId).children(".close-tips");
		if (A) {
			_.show();
		} else {
			_.hide();
		}
		_.click(function () {
			B.setVisible(false);
		});
	};
	disk.ui.Toast = _;
})();
(function () {
	var _ = function () {
		this._mTabIndicators = [];
		this._mTabContents = [];
		this._mSelectedIndex = -1;
		this._mOnClass = "on";
		var _ = this;
		this._mTabClickListener = function () {
			var A = parseInt(this.getAttribute("_idx"));
			_.setSelected(A);
			return false;
		};
	};
	_.prototype = {
		setSelected : function (_, A) {
			if (_ == this._mSelectedIndex) {
				return false;
			}
			if (this.onInterfereTabChange(this._mSelectedIndex, _)) {
				return;
			}
			this._uiSetSelected(this._mSelectedIndex, false);
			this._uiSetSelected(_, true);
			this.onTabChange(this._mSelectedIndex, _);
			this._mSelectedIndex = _;
		},
		_uiSetSelected : function (A, B) {
			var _ = this._mTabIndicators[A],
			C = this._mTabContents[A];
			if (B) {
				if (_) {
					$(_).addClass(this._mOnClass);
				}
				if (C) {
					$(C).css("display", "block");
				}
			} else {
				if (_) {
					$(_).removeClass(this._mOnClass);
				}
				if (C) {
					$(C).css("display", "none");
				}
			}
		},
		addTab : function (_, A) {
			_.setAttribute("_idx", this._mTabIndicators.length);
			this._mTabIndicators.push(_);
			this._mTabContents.push(A);
			_.onclick = this._mTabClickListener;
		},
		setOnClass : function (_) {
			this._mOnClass = _;
		},
		selectedIndex : function () {
			return this._mSelectedIndex;
		},
		onTabChange : function (_, A) {},
		onInterfereTabChange : function (_, A) {
			return false;
		}
	};
	disk.ui.SimpleTabHost = _;
})();
disk.ui.Utilities = {
	TREENODE_EXTRA_SPACE_PIXEL : 0,
	IGNORE_PARTICULAR_DIR : true,
	IGNORE_EMPTY_DIR : false,
	_mTreeSelectDir : "",
	BETA_INFO : null,
	globalTreeNodeSelectHandler : null,
	_mOptionConf : {},
	setGlobalTreeNodeSelectHandler : function (_) {
		this.globalTreeNodeSelectHandler = _;
	},
	handleTreeNodeSelect : function (C, _, A, B) {
		var D = this;
		if (B === true && !disk.ui.Utilities.IGNORE_EMPTY_DIR) {
			if (disk.ui.Utilities.globalTreeNodeSelectHandler != null && A !== true) {
				disk.ui.Utilities.globalTreeNodeSelectHandler.call(C, C);
			}
			return;
		}
		if (disk.DEBUG) {
			console.log("handeTreeNodeSelect");
		}
		if (C.getAttr("build")) {
			disk.ui.Utilities._mTreeSelectDir = C.getAttr("dir");
			if (typeof _ == "function") {
				_.call(this, C);
			}
			if (disk.ui.Utilities.globalTreeNodeSelectHandler != null && A !== true) {
				disk.ui.Utilities.globalTreeNodeSelectHandler.call(C, C);
			}
			return;
		}
		disk.ui.Utilities._mTreeSelectDir = undefined;
		C.setLoading(true);
		FileUtils.loadDir(C.getAttr("dir"), function (D) {
			if (typeof disk.ui.Utilities._mTreeSelectDir == "undefined" && disk.ui.Utilities.globalTreeNodeSelectHandler != null && A !== true) {
				disk.ui.Utilities.globalTreeNodeSelectHandler.call(C, C);
			}
			C.setAttr("build", true);
			C.setLoading(false);
			var H = null,
			I = null,
			D = D || [];
			for (var E = 0, B = D.length; E < B; E++) {
				H = D[E];
				I = H.path;
				if (H.isdir) {
					var G = FileUtils.isInParticularDir(FileUtils.parseDirPath(I)),
					F = new disk.ui.TreeNode(C, {
							text : FileUtils.getNormalizedPath(FileUtils.parseDirPath(I), true),
							onSelect : disk.ui.Utilities.handleTreeNodeSelect,
							onExpand : disk.ui.Utilities.handleTreeNodeExpand,
							empty : H.empty,
							dir_empty : H.dir_empty,
							isParti : G
						});
					F.setAttr("dir", FileUtils.parseDirPath(I));
				}
			}
			if (D.length > 0) {
				C.expand(true);
			}
			if (typeof _ == "function") {
				_.call(this, C);
			}
		});
	},
	handleTreeNodeExpand : function (C, B) {
		if (disk.DEBUG) {
			console.log("node expanded " + C);
		}
		var A = disk.ui.UploadDialog.active,
		_ = disk.ui.MoveCopyDialog.active;
		if (A != null && A.isVisible()) {
			A.requestLayoutTreeview();
		} else {
			if (_ != null && _.isVisible()) {
				_.requestLayoutTreeview();
			}
		}
	},
	selectTextField : function (B, _, A) {
		if (B.createTextRange) {
			var C = B.createTextRange();
			C.collapse(true);
			C.moveStart("character", _);
			C.moveEnd("character", A);
			C.select();
		} else {
			if (B.setSelectionRange) {
				B.setSelectionRange(_, A);
			} else {
				if (B.selectionStart) {
					B.selectionStart = _;
					B.selectionEnd = A;
				}
			}
		}
		B.focus();
	},
	getThumbnail : function (D, F) {
		F = $.extend({
				width : 115,
				height : 115
			}, F);
		if (typeof D.thumbs !== "undefined" && typeof D.thumbs.url1 !== "undefined") {
			var C = D.thumbs.url1;
			if (C.indexOf("&size=c140_u90") > 0) {
				return C.replace("&size=c140_u90", "&size=c" + F.width + "_u" + F.height);
			}
		}
		var E = disk.api.RestAPI.THUMBNAIL_GENERATE,
		B = {
			path : D.path,
			width : F.width,
			height : F.height,
			quality : 100
		};
		for (var _ in B) {
			var A = encodeURIComponent(B[_]);
			E += "&" + _ + "=" + A;
		}
		return E;
	},
	bringNodeToFrontInTreeview : function (A, E) {
		var H = (typeof E != "undefined") ? E : disk.ui.TreeLeaf.activeLeaf;
		if (H == null) {
			return;
		}
		var _ = A.offset(),
		D = $("span", H.getNode()).offset(),
		C = $(H.getNode()).height(),
		B = $(H.getNode()).width(),
		G = A.height();
		if (D.top + C - _.top > G * 4 / 5) {
			A.scrollTop(A.scrollTop() + D.top - _.top - G + C + G / 2);
			if (disk.DEBUG) {
				console.log(">>>>scroll down to become visible");
			}
		} else {
			if (D.top - _.top < 0) {
				A.scrollTop(A.scrollTop() - Math.abs(_.top - D.top + G / 2));
				if (disk.DEBUG) {
					console.log(">>>>scroll up to become visible");
				}
			}
		}
		if (H.getHost().getDeep() < 3) {
			return;
		}
		var F = D.left - _.left;
		if (F > 0) {
			A.scrollLeft(A.scrollLeft() + D.left - _.left + 20);
			if (disk.DEBUG) {
				console.log(">>>>>scroll right to become visible");
			}
		} else {
			if (F < 0) {
				A.scrollLeft(A.scrollLeft() - Math.abs(_.left - D.left));
				if (disk.DEBUG) {
					console.log(">>>>>scroll left to become visible");
				}
			}
		}
	},
	encodeHTML : function (_) {
		return String(_).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&#39;");
	},
	getFlashVersion : function () {
		var f = null,
		n = navigator;
		if (n.plugins && n.plugins.length) {
			for (var ii = 0, len = n.plugins.length; ii < len; ii++) {
				if (n.plugins[ii].name.indexOf("Shockwave Flash") != -1) {
					f = n.plugins[ii].description.split("Shockwave Flash ")[1];
					break;
				}
			}
		} else {
			if (window.ActiveXObject) {
				for (ii = 11; ii >= 2; ii--) {
					try {
						var fl = eval("new ActiveXObject('ShockwaveFlash.ShockwaveFlash." + ii + "');");
						if (fl) {
							f = ii + ".0";
							break;
						}
					} catch (e) {}

				}
			}
		}
		return f;
	},
	_testify0 : function (_) {
		var A = this.BETA_INFO.expvar,
		B = false;
		if (A && A.length > 0) {
			if ($.inArray(_, A) >= 0) {
				B = true;
			}
		}
		return B;
	},
	getBetaFeature : function (_) {
		if (!document || !document.body) {
			return false;
		}
		if (this.BETA_INFO != null) {
			return this._testify0(_);
		}
		var A = disk.ui.Utilities.BETA_STR;
		if (A) {
			var B = $.parseJSON(A);
			if (!B) {
				return false;
			}
			this.BETA_INFO = B;
			return this._testify0(_);
		} else {
			return false;
		}
	},
	getBetaFeatureByPersonal : function () {
		return this.getBetaFeature("exp_pub");
	},
	getBetaFeatureByDevice : function () {
		return this.getBetaFeature("device_pub");
	},
	getBetaFeatureByEmule : function () {
		return this.getBetaFeature("emule_pub");
	},
	getBetaFeatureByMagnet : function () {
		return this.getBetaFeature("magnet_pub");
	},
	getBetaFeatureByVideoFlash : function () {
		return this.getBetaFeature("sharevideo_pub");
	},
	getBetaFeatureByTimeLine : function () {
		return this.getBetaFeature("timeline_pub");
	},
	_html5Compatible : function () {
		return (typeof FormData != "undefined");
	},
	execOptionQuery : function () {
		var _ = this._mOptionConf;
		if (_.progress > 0) {}
		else {
			if (this._html5Compatible()) {
				var A = new XMLHttpRequest();
				A.onreadystatechange = function (B) {
					if (B.target.readyState == 4) {
						if (disk.DEBUG) {
							console.log("readyState=", A.readyState, "status=", A.status);
						}
						FileUtils._mDiskLog.send({
							type : "html5optionsstatus",
							value : B.target.status
						});
						_.progress = 2;
						if (typeof localStorage != "undefined" && typeof localStorage.getItem != "undefined") {
							localStorage.setItem("pref_html_option_request", B.target.status == 200 ? "1" : "0");
						}
					}
				};
				A.open("OPTIONS", "https://c.pcs.baidu.com/rest/2.0/pcs/file", true);
				A.send(null);
				_.progress = 1;
			}
		}
	},
	getSuggestedOptionQuery : function () {
		if (!this._html5Compatible()) {
			return false;
		}
		if (typeof localStorage == "undefined" || typeof localStorage.getItem == "undefined") {
			return false;
		}
		var _ = localStorage.getItem("pref_html_option_request");
		if (_ == null || typeof _ == "undefined") {
			return false;
		}
		return _ == "1";
	}
};
(function () {
	var _ = function () {
		this._mTitleId = disk.obtainId();
		this._mMsgContentId = disk.obtainId();
		this._mCloseId = disk.obtainId();
		disk.ui.Panel.call(this, this._prebuild(this._mTitleId, this._mMsgContentId, this._mCloseId));
		this._mOnConsent = null;
		this._mOnCancel = null;
		this._mViewFlags = 0;
		this.build();
	};
	_.NOT_PREVENT_DEFAULT = 1;
	_.prototype = new disk.ui.Panel();
	_.prototype._prebuild = function (B, A, C) {
		var _ = document.createElement("div");
		_.className = "b-panel b-dialog alert-dialog";
		document.body.appendChild(_);
		_.innerHTML = '<div class="dlg-hd b-rlv">' + '<div title="\u5173\u95ed" id="' + C + '" class="dlg-cnr dlg-cnr-r"></div><h3 id="' + B + '">\u786e\u8ba4\u64cd\u4f5c</h3></div><div class="dlg-bd"><div id="' + A + '" class="alert-dialog-msg center"></div></div><div class="dlg-ft b-rlv"><div class="alert-dialog-commands clearfix center">' + '<a href="javascript:;" class="sbtn okay"><b>\u786e\u5b9a</b></a>' + '<a href="javascript:;" class="dbtn cancel"><b>\u53d6\u6d88</b></a></div>' + '<div class="clearfix alert-dialog-commands-plus" ><a href="javascript:;" class="dbtn okay"><b>\u5173\u95ed</b></a></div>' + "</div>";
		return {
			pane : _
		};
	};
	_.prototype.setVisible = function (A, _) {
		disk.ui.Panel.prototype.setVisible.call(this, A);
		if (A) {
			if (_ && typeof _ === "string") {
				if ($(this._mUI.pane).hasClass("share_update_dialog") === false) {
					$(this._mUI.pane).addClass("share_update_dialog");
				}
			} else {
				if ($(this._mUI.pane).hasClass("share_update_dialog")) {
					$(this._mUI.pane).removeClass("share_update_dialog");
				}
			}
			this.setGravity(disk.ui.Panel.CENTER);
		}
	};
	_.prototype.onVisibilityChange = function (B) {
		var A = disk.Context.getService(disk.Context.SERVICE_CANVAS);
		A.setVisible(B);
		if (!B) {
			this._mViewFlags &= ~_.NOT_PREVENT_DEFAULT;
		}
	};
	_.prototype.setMessage = function (B, A, _, C) {
		this._mOnConsent = _ || null;
		this._mOnCancel = C || null;
		$("#" + this._mTitleId).html(B);
		$("#" + this._mMsgContentId).html(A);
		this.setGravity(disk.ui.Panel.CENTER);
		return this;
	};
	_.prototype.setNotPreventDefault = function (A) {
		if (A) {
			this._mViewFlags |= _.NOT_PREVENT_DEFAULT;
		} else {
			this._mViewFlags &= ~_.NOT_PREVENT_DEFAULT;
		}
	};
	_.prototype.build = function () {
		var A = this;
		$(window).bind("resize", function (_) {
			A.setGravity(disk.ui.Panel.CENTER);
		});
		$(".okay", this._mUI.pane).click(function (B) {
			if (A._mOnConsent == null) {
				A.setVisible(false);
			} else {
				A._mOnConsent.call(A);
			}
			if ((A._mViewFlags & _.NOT_PREVENT_DEFAULT) != _.NOT_PREVENT_DEFAULT) {
				return false;
			}
		});
		$(".cancel, #" + this._mCloseId, this._mUI.pane).click(function (B) {
			A.setVisible(false);
			if (typeof A._mOnCancel == "function") {
				A._mOnCancel.call(A, $(this));
			}
			if ((A._mViewFlags & _.NOT_PREVENT_DEFAULT) != _.NOT_PREVENT_DEFAULT) {
				return false;
			}
		});
	};
	disk.ui.AlertDialog = _;
})();
(function () {
	var _ = function () {};
	_.canvas = null;
	_.dialog = null;
	_.toast = null;
	_.treeView = null;
	_.treeViewRoot = null;
	_.downloadProxy = null;
	_.keyguard = null;
	_.promotion = null;
	_.PersonalEditDialog = null;
	_.APPS_POOL = [];
	_.SERVICE_CANVAS = 1;
	_.SERVICE_DIALOG = 2;
	_.SERVICE_SELECT_DIALOG = 64;
	_.SERVICE_TOAST = 4;
	_.SERVICE_DOWNLOAD_PROXY = 8;
	_.SERVICE_KEYGUARD = 16;
	_.SERVICE_PROMOTION = 32;
	_.SERVICE_PERSONALEDIT_DIALOG = 48;
	_.SERVICE_FILLNAME_DIALOG = 80;
	_.FEATURE_TREEVIEW = 1;
	_.FEATURE_STORAGE = 2;
	_.getService = function (A) {
		switch (A) {
		case _.SERVICE_CANVAS:
			if (_.canvas == null) {
				_.canvas = new disk.ui.Canvas();
			}
			return _.canvas;
			break;
		case _.SERVICE_DIALOG:
			if (_.dialog == null) {
				_.dialog = new disk.ui.AlertDialog();
			}
			return _.dialog;
		case _.SERVICE_SELECT_DIALOG:
			if (_.select_dialog == null) {
				_.select_dialog = new disk.ui.SelectDialog();
			}
			return _.select_dialog;
		case _.SERVICE_TOAST:
			if (_.toast == null) {
				_.toast = new disk.ui.Toast();
			}
			return _.toast;
		case _.SERVICE_DOWNLOAD_PROXY:
			if (_.downloadProxy == null) {
				_.downloadProxy = new disk.util.DownloadProxy();
			}
			return _.downloadProxy;
		case _.SERVICE_KEYGUARD:
			if (_.keyguard == null) {
				_.keyguard = new disk.ui.Keyguard();
			}
			return _.keyguard;
		case _.SERVICE_PROMOTION:
			if (_.promotion == null) {
				_.promotion = new disk.ui.PromotionService();
			}
			return _.promotion;
		case _.SERVICE_PERSONALEDIT_DIALOG:
			if (_.PersonalEditDialog == null) {
				_.PersonalEditDialog = new disk.ui.PersonalEditDialog();
			}
			return _.PersonalEditDialog;
		case _.SERVICE_FILLNAME_DIALOG:
			if (_.FillUserNameDialog == null) {
				_.FillUserNameDialog = new disk.ui.FillUserNameDialog();
			}
			return _.FillUserNameDialog;
		}
	};
	_.requestFeature = function (D, C) {
		C = C || {};
		switch (D) {
		case _.FEATURE_STORAGE:
			if (typeof localStorage == "undefined") {
				$.getScript("/res/static/js/util/storage.js").done(C.succ).fail(C.fail);
			} else {
				C.succ.call(C.succ);
			}
			break;
		case _.FEATURE_TREEVIEW:
			var E = C.host,
			A = C.newDirectoryTrigger;
			if (!disk.Context.treeView || C.rebuild) {
				if (disk.DEBUG) {
					console.log("build the treeview again");
				}
				E.innerHTML = "";
				var G = new disk.ui.TreeNode(null, {
						root : true,
						treeContentCss : "treeview-ancestor",
						host : E,
						dir_empty : 0,
						empty : 0
					}),
				J = new disk.ui.TreeNode(G, {
						text : "\u5168\u90e8\u6587\u4ef6",
						css : "treeview-root",
						onSelect : disk.ui.Utilities.handleTreeNodeSelect,
						onExpand : disk.ui.Utilities.handleTreeNodeExpand,
						dir_empty : 0,
						empty : 0
					});
				J.setAttr("dir", FileUtils.ROOT_ID);
				G.expand(true);
				disk.Context.treeView = G;
				disk.Context.treeViewRoot = J;
			} else {
				E.appendChild(disk.Context.treeView.getNode());
			}
			if (typeof C.path != "undefined" && C.path != null) {
				var I = C.path,
				H = disk.Context.treeViewRoot;
				if (I == disk.util.DirectoryLocation.DIR_ROOT) {
					disk.ui.Utilities.handleTreeNodeSelect(H, function (_) {
						var A = _.getLeaf();
						if (disk.ui.TreeLeaf.activeLeaf != null && disk.ui.TreeLeaf.activeLeaf != A) {
							disk.ui.TreeLeaf.activeLeaf.setSelected(false);
						}
						A.setSelected(true);
						disk.ui.TreeLeaf.activeLeaf = A;
						if (disk.DEBUG) {
							console.log("found leaf and select it at case2");
						}
						if (typeof C.callback == "function") {
							C.callback.call(this);
						}
					});
				} else {
					var B = disk.util.DirectoryLocation.toDirArray(I);
					B.shift();
					var F = function (E) {
						if (B.length == 0) {
							return;
						}
						var H = B.shift();
						for (var D = 0, A = E.children(), _ = A.length; D < _; D++) {
							if (A[D].getAttr("dir") == H) {
								if (B.length == 0) {
									var G = A[D].getLeaf();
									if (disk.DEBUG) {
										console.assert(G instanceof disk.ui.TreeLeaf);
									}
									if (G != disk.ui.TreeLeaf.activeLeaf && disk.ui.TreeLeaf.activeLeaf != null) {
										disk.ui.TreeLeaf.activeLeaf.setSelected(false);
									}
									G.setSelected(true);
									disk.ui.TreeLeaf.activeLeaf = G;
									if (disk.DEBUG) {
										console.log("found leaf and select it at case2");
									}
									if (typeof C.callback == "function") {
										C.callback.call(this);
									}
								}
								disk.ui.Utilities.handleTreeNodeSelect(A[D], F);
							}
						}
					};
					disk.ui.Utilities.handleTreeNodeSelect(H, F);
				}
			}
			if (A.attr("_installed")) {
				return;
			}
			A.click(function (B) {
				var A = disk.ui.FolderBuilder.getDefault();
				if (A.isVisible()) {
					A.focus();
					A.select();
					return;
				}
				var _ = disk.ui.TreeLeaf.activeLeaf != null ? disk.ui.TreeLeaf.activeLeaf.getHost() : disk.Context.treeViewRoot;
				disk.ui.Utilities.handleTreeNodeSelect(_, function () {
					if (A.isVisible()) {
						if (disk.DEBUG) {
							console.log("[JAZZ] a weird bug, will create more than one pending folder");
						}
						return;
					}
					_.expand(true);
					var B = new disk.ui.TreeNode(_, {
							onSelect : disk.ui.Utilities.handleTreeNodeSelect,
							text : "",
							onExpand : disk.ui.Utilities.handleTreeNodeExpand,
							dir_empty : 1,
							empty : 1
						}),
					C = function (C, _) {
						if (C) {
							A.attach(_, function (C, _) {
								var F = this;
								if (C == disk.ui.FolderBuilder.POSITIVE) {
									B._mLeaf.setLoading(true);
									var D = B.getParent().getAttr("dir"),
									A = (D == FileUtils.ROOT_ID) ? "/" + _ : D + "/" + _;
									FileUtils.sendCreateNewFileMessage(A, "", 1, null, function (J) {
										B._mLeaf.setLoading(false);
										try {
											var I = $.parseJSON(J),
											H = FileUtils.parseNameFromPath(I.path),
											G = "0:" + H;
											_ = FileUtils.parseDirFromPath(I.path);
											if (I.errno == 0) {
												F.setVisible(false);
												F.detach();
												B._mLeaf.edit(false);
												B._mLeaf.setText(_);
												A = FileUtils.resolveNewName(true, A, _, true);
												B.setAttr("dir", A);
												B.getParent().getLeaf().setAsEmpty(false);
												disk.ui.Utilities.bringNodeToFrontInTreeview($(E), B.getLeaf());
												B.getLeaf().requestFocus();
												var C = I.modify_time,
												K = FileUtils.resolveParentDirSharedProperty(D);
												if (disk.DEBUG) {
													console.log("Resolve Dir ", D, " = ", K);
												}
												FileUtils.getLocalCache().insert(D, {
													isdir : 1,
													ctime : I.ctime,
													modify_time : I.modify_time,
													mtime : I.mtime,
													path : I.path,
													status : I.status,
													fs_id : I.fs_id,
													size : 0,
													dir_empty : 1,
													empty : 1
												});
												FileUtils.resolveParentDirEmptyState(D);
											} else {
												if (I.errno == -8) {
													Utilities.useToast({
														toastMode : disk.ui.Toast.MODE_CAUTION,
														msg : "\u540d\u79f0\u5df2\u5728\u4f7f\u7528\uff0c\u8bf7\u4f7f\u7528\u5176\u4ed6\u540d\u79f0",
														sticky : false
													});
												} else {
													F.setVisible(false);
													F.detach();
													B.remove();
													Utilities.useToast({
														toastMode : disk.ui.Toast.MODE_CAUTION,
														msg : "\u6587\u4ef6\u5939\u521b\u5efa\u5931\u8d25",
														sticky : false
													});
												}
											}
										} catch (L) {
											F.setVisible(false);
											F.detach();
											B.remove();
											Utilities.useToast({
												toastMode : disk.ui.Toast.MODE_CAUTION,
												msg : "\u6587\u4ef6\u5939\u521b\u5efa\u5931\u8d25",
												sticky : false
											});
										}
									});
								} else {
									if (C == disk.ui.FolderBuilder.ILLEGAL) {
										Utilities.useToast({
											toastMode : disk.ui.Toast.MODE_CAUTION,
											msg : "\u6587\u4ef6\u540d\u683c\u5f0f\u4e0d\u6b63\u786e",
											sticky : false
										});
									} else {
										B.remove();
									}
								}
							});
							A.setVisible(true);
							A._mTextField.removeAttr("style");
							A.setUseShortStyle(true, true);
							if (disk.DEBUG) {
								console.log("before schedule a adjusting event");
							}
							disk.ui.Utilities.bringNodeToFrontInTreeview($(E), B.getLeaf());
						} else {
							A.setVisible(false);
							A.setUseShortStyle(false);
							A.detach();
						}
					};
					B._mLeaf.edit(true, C);
				});
				A.focus();
				A.select();
				return false;
			});
			break;
		}
	};
	var A = function () {
		this._mName = null;
		this._mIFrame = null;
		this._mURL = null;
		this._mVisible = false;
		this._mActive = false;
		this._mRequestTimer = null;
		this._mCallback = null;
		this._buildIFrame();
	};
	A.appZIndex = 1999;
	A.ERR_TIMEOUT = 0;
	A.ACTION_VISIBILITY_CHANGED = 1;
	A.ACTION_USER_DEFINED = 100;
	A.requestApp = function (H, F, D) {
		var C = false,
		E = null,
		G = null;
		for (var B = 0, A = _.APPS_POOL.length; B < A; B++) {
			E = _.APPS_POOL[B];
			E.setActive(false);
			if (E.getName() == H) {
				G = E;
			}
		}
		if (G == null) {
			G = new _.EmbedApp();
			G.setName(H);
		}
		G.setURL(F);
		G.setCallback(D);
		_.APPS_POOL.push(G);
	};
	A.requestActivateApp = function (D) {
		var C = null;
		for (var B = 0, A = _.APPS_POOL.length; B < A; B++) {
			C = _.APPS_POOL[B];
			if (C.getName() == D) {
				C.setActive(true);
				C.setVisible(true);
				break;
			}
		}
	};
	A.requestDeactivateApp = function (D) {
		var C = null;
		for (var B = 0, A = _.APPS_POOL.length; B < A; B++) {
			C = _.APPS_POOL[B];
			if (C.getName() == D) {
				C.setActive(false);
				C.setVisible(false);
				if (B - 1 >= 0) {
					_.APPS_POOL[B - 1].setActive(true);
				}
				_.APPS_POOL.splice(B, 1);
				break;
			}
		}
	};
	A.requestDeactivateFrontApp = function () {
		var B = _.APPS_POOL;
		if (B.length > 0) {
			A.requestDeactivateApp(B[B.length - 1].getName());
		}
	};
	A.queryApp = function (D) {
		var C = null;
		for (var B = 0, A = _.APPS_POOL.length; B < A; B++) {
			C = _.APPS_POOL[B];
			if (C.getName() == D) {
				return C;
			}
		}
		return null;
	};
	A.dispatchResizeWindow = function (D) {
		var C = null;
		for (var B = 0, A = _.APPS_POOL.length; B < A; B++) {
			C = _.APPS_POOL[B];
			if ((typeof D == "undefined") || (C.getName() == D)) {
				if (C.isActive()) {
					C.dispatchResizeWindow();
				}
			}
		}
	};
	A.layoutSubwindow = function (A) {
		var _ = $("#subwindowContainer");
		A.style.height = _.height() + "px";
		A.style.top = "61px";
		if ($(window).width() < 1024) {
			A.style.left = _.offset().left + "px";
			A.style.width = _.outerWidth() + "px";
		} else {
			A.style.left = (_.offset().left + 2) + "px";
			A.style.width = (_.outerWidth() - 2) + "px";
		}
		A.style.position = "absolute";
		A.style.background = "#fff";
		$("body").append(A);
	};
	A.prototype = {
		_buildIFrame : function () {
			if (this._mIFrame) {
				return;
			}
			var _ = document.createElement("iframe");
			_.frameBorder = "0";
			_.style.zIndex = 12;
			_.style.display = "none";
			$("body").append(_);
			this._mIFrame = _;
		},
		dispatchResizeWindow : function () {
			if (this._mIFrame != null) {
				disk.Context.EmbedApp.layoutSubwindow(this._mIFrame);
			}
			try {
				this._mIFrame.contentWindow.PageUtils.dispatchResizeWindow();
			} catch (_) {}

		},
		setVisible : function (A, _) {
			var C = this,
			B = 400;
			if (A) {
				this._mIFrame.style.display = "block";
				this.dispatchResizeWindow();
				C._mActive = true;
				disk.util.HistoryManager.getDefault().listen(C._mName, function (_) {
					if (!C._mActive) {
						return;
					}
				});
				if (typeof _ == "function") {
					_.call(C);
				}
				if (typeof C._mCallback == "function") {
					C._mCallback.call(C, disk.Context.EmbedApp.ACTION_VISIBILITY_CHANGED, true);
				}
			} else {
				C._mActive = true;
				C._mIFrame.style.display = "none";
				C._mIFrame.src = "";
				C._mIFrame.parentNode.removeChild(C._mIFrame);
				disk.util.HistoryManager.getDefault().unlisten(C._mName);
				if (typeof _ == "function") {
					_.call(C);
				}
				if (typeof C._mCallback == "function") {
					C._mCallback.call(C, disk.Context.EmbedApp.ACTION_VISIBILITY_CHANGED, false);
				}
			}
		},
		setName : function (_) {
			this._mName = _;
		},
		getName : function () {
			return this._mName;
		},
		setURL : function (_) {
			this._mURL = _;
			this._mIFrame.src = _;
			var A = this;
			if (!this._mRequestTimer) {
				this._mRequestTimer = new disk.util.TimerService(30 * 1000, function () {
						disk.Context.EmbedApp.requestDeactivateApp(A.getName());
						if (typeof A._mCallback == "function") {
							A._mCallback.call(A, disk.Context.EmbedApp.ERR_TIMEOUT);
						}
					});
			} else {
				this._mRequestTimer.interrupt();
			}
			this._mRequestTimer.start();
		},
		isActive : function () {
			return this._mActive;
		},
		dispatchSystemMessage : function (_) {
			try {
				this._mIFrame.contentWindow.PageUtils.dispatchSystemMessage(_);
				return true;
			} catch (A) {
				return false;
			}
		},
		dispatchCallbackMessage : function (A, _) {
			if (typeof this._mCallback == "function") {
				this._mCallback.call(this, A, _);
			}
		},
		setActive : function (_) {
			this._mActive = _;
			if (_ == true && this._mRequestTimer != null) {
				this._mRequestTimer.interrupt();
				this._mRequestTimer = null;
			}
		},
		setCallback : function (_) {
			this._mCallback = _;
		}
	};
	_.EmbedApp = A;
	disk.Context = _;
})();
(function () {
	var _ = function () {
		this._mOnConsent = null;
		this._mGeneratorUI = null;
		this._mTextField = null;
		this._mPrecedeHost = null;
		this._confirmHandler = null;
		this._mPrivateFlags = 0;
	};
	_.SHORT_STYLE = 1;
	_.VISIBLE = 2;
	_.IGNORE_TRAILING_EXT = 4;
	_.POSITIVE = 4;
	_.NEGETIVE = 8;
	_.ILLEGAL = 16;
	_.ILLEGAL_CHARS = /[\\\/\:\*\?"<>|]/gi;
	_.instance = null;
	_.getDefault = function () {
		return _.instance ? _.instance : (_.instance = new _());
	};
	_.prototype = {
		attach : function (_, A) {
			this._mOnConsent = (typeof A == "function") ? A : null;
			this._maybeBuildUI();
			this._mPrecedeHost = _;
			_.appendChild(this._mGeneratorUI);
			this._mGeneratorUI.style.display = "block";
			this.onAttach(true, _);
			return this;
		},
		detach : function () {
			document.body.appendChild(this._mGeneratorUI);
			this._mGeneratorUI.style.display = "none";
			this.onAttach(false, this._mPrecedeHost);
		},
		getDOM : function () {
			return this._mGeneratorUI;
		},
		_maybeBuildUI : function () {
			if (this._mGeneratorUI == null) {
				this._mGeneratorUI = document.createElement("form");
				document.body.appendChild(this._mGeneratorUI);
				this._mGeneratorUI.style.display = "none";
				this._mGeneratorUI.action = "javascript:;";
				this._mGeneratorUI.method = "get";
				this._mGeneratorUI.name = "folderGeneratorForm";
				this._mGeneratorUI.id = "folderGeneratorFormId";
				this._mGeneratorUI.className = "folder-generator file-handler";
				this._mGeneratorUI.innerHTML = '<input id="folderGeneratorHandler" autocomplete="off" spellcheck="false" type="text" name="generator"><a title="\u786e\u5b9a" class="b-in-blk img-ico ic-chname-ok" href="javascript:;"></a><a title="\u53d6\u6d88" class="b-in-blk img-ico ic-chname-failure" href="javascript:;"></a>';
				this._mTextField = $("input", this._mGeneratorUI);
				var B = this,
				A = function (E) {
					var D = B._mTextField[0],
					A = D.value;
					_.ILLEGAL_CHARS.lastIndex = 0;
					var C = _.ILLEGAL_CHARS.test(A);
					if (B._mOnConsent != null) {
						if (A.trim().length == 0 || C || A.indexOf(".") == 0) {
							B._mOnConsent.call(B, disk.ui.FolderBuilder.ILLEGAL, null);
						} else {
							if (B.checkFileNameMaxLength(A) === false) {}
							else {
								B._mOnConsent.call(B, disk.ui.FolderBuilder.POSITIVE, A.trim());
								E.preventDefault();
								E.stopPropagation();
							}
						}
					}
				};
				this._confirmHandler = A;
				$(".ic-chname-ok", this._mGeneratorUI).click(this._confirmHandler);
				$(this._mGeneratorUI).submit(this._confirmHandler);
				$(".ic-chname-failure", this._mGeneratorUI).click(function (_) {
					B.setVisible(false);
					if (B._mOnConsent != null) {
						B._mOnConsent.call(B, disk.ui.FolderBuilder.NEGETIVE, null);
					}
				});
				$(this._mGeneratorUI).click(function (_) {
					_.preventDefault();
					_.stopPropagation();
					return false;
				});
			}
		},
		checkFileNameMaxLength : function (C) {
			var B = C;
			for (var _ = 0, A = 0; _ < B.length; _++) {
				if (B.charCodeAt(_) < 128) {
					A++;
				} else {
					A += 2;
				}
			}
			if (A > 255) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u6587\u4ef6(\u5939)\u540d\u79f0\u957f\u5ea6\u4e0d\u80fd\u8d85\u8fc7255\u5b57\u8282",
					sticky : false
				});
				return false;
			} else {
				return true;
			}
		},
		dispatchNegativeAction : function () {
			var _ = this;
			_.setVisible(false);
			if (_._mOnConsent != null) {
				_._mOnConsent.call(_, disk.ui.FolderBuilder.NEGETIVE, null);
			}
		},
		setUseShortStyle : function (A, _) {
			if (A) {
				this._mPrivateFlags |= disk.ui.FolderBuilder.SHORT_STYLE;
				if (_) {
					this._mTextField.css({
						"margin-left" : "-17px"
					});
				} else {
					this._mTextField.css({
						position : "relative",
						left : "-17px"
					});
				}
				this._mTextField.attr("size", 10);
			} else {
				this._mPrivateFlags &= ~disk.ui.FolderBuilder.SHORT_STYLE;
				this._mTextField.attr("size", 35);
			}
		},
		focus : function () {
			this._mTextField.focus();
		},
		setIgnoreTrailingExt : function (_) {
			if (_) {
				this._mPrivateFlags |= disk.ui.FolderBuilder.IGNORE_TRAILING_EXT;
			} else {
				this._mPrivateFlags &= ~disk.ui.FolderBuilder.IGNORE_TRAILING_EXT;
			}
		},
		select : function () {
			var A = this._mTextField[0],
			B = this._mTextField.attr("value"),
			_ = B.lastIndexOf(".");
			if ((this._mPrivateFlags & disk.ui.FolderBuilder.IGNORE_TRAILING_EXT) == disk.ui.FolderBuilder.IGNORE_TRAILING_EXT) {
				_ = -1;
			}
			if (B.trim().length > 0 && _ != -1) {
				disk.ui.Utilities.selectTextField(A, 0, _);
				if (disk.DEBUG) {
					console.log("before select text field between index 0 and ", _);
				}
			} else {
				this._mTextField.select();
			}
		},
		setText : function (_) {
			var A = this._mTextField[0];
			A.value = _;
		},
		setVisible : function (_) {
			if (_) {
				this._mPrivateFlags |= disk.ui.FolderBuilder.VISIBLE;
				$(this._mGeneratorUI).css("display", "");
				this.setText("\u65b0\u5efa\u6587\u4ef6\u5939");
			} else {
				this._mPrivateFlags &= ~disk.ui.FolderBuilder.VISIBLE;
				$(this._mGeneratorUI).css("display", "none");
				this._mPrivateFlags &= ~disk.ui.FolderBuilder.IGNORE_TRAILING_EXT;
			}
			return this;
		},
		isVisible : function () {
			return (this._mPrivateFlags & disk.ui.FolderBuilder.VISIBLE) == disk.ui.FolderBuilder.VISIBLE;
		},
		onAttach : function (A, _) {}

	};
	disk.ui.FolderBuilder = _;
})();
(function () {
	var _ = function (A, _) {
		this._mUI = A;
		this._mConfig = _ || {};
		this._mThumbTop = 0;
		this._mClickTimer = null;
		this._mPrivateFlags = 0;
		this._mCoorX = -1;
		this._mCoorY = -1;
		this._mCoorWidth = 0;
		this._mCoorHeight = 0;
		this._mArrowScrollDir = -1;
		this._mDX = -1;
		this._mDY = -1;
		this._init();
	};
	_.FORWARD = 1;
	_.BACKWARD = 2;
	_.AWAKE = 4;
	_.THUMB_AWAKE = 8;
	_.COUNTING = 16;
	_.USING_SIMULATE_DRAG = 32;
	_.START_DRAGGING = 64;
	_.LOCKED = 128;
	_.HAS_BORDER = 256;
	_.CLICK_TIMEOUT = 300;
	_.prototype = {
		_init : function () {
			this._mPrivateFlags |= _.AWAKE;
			this._mPrivateFlags |= _.THUMB_AWAKE;
			this._mPrivateFlags |= _.HAS_BORDER;
			var F = this;
			if (this._mUI.tracker) {
				this._mUI.tracker.onclick = function (C) {
					if (F.locked()) {
						return;
					}
					var A = C ? C : window.event,
					_ = A.target || A.srcElement;
					if (_ == F._mUI.thumb) {
						return;
					}
					var B = parseInt(A.offsetY || A.layerY);
					if (B < F._mThumbTop || B > F._mThumbTop + F._mUI.thumb.offsetHeight) {
						if (B > F._mThumbTop) {
							F.onPageScroll(disk.ui.MouseWheelSensor.FORWARD);
						} else {
							if (B < F._mThumbTop) {
								F.onPageScroll(disk.ui.MouseWheelSensor.BACKWARD);
							}
						}
					}
				};
			}
			var C = function (G) {
				if ((F._mPrivateFlags & _.COUNTING) == _.COUNTING) {
					var A = G ? G : window.event,
					E = A.clientX,
					B = A.clientY;
					if (E < F._mCoorX || E > F._mCoorX + F._mCoorWidth || B < F._mCoorY || B > F._mCoorY + F._mCoorHeight) {
						F.abruptPersistArrowScroll();
						F.onPersistArrowScrollEnd();
						F._unlisten(document, "mousemove", C);
						F._unlisten(document, "mouseup", D);
					}
				}
			},
			D = function () {
				if ((F._mPrivateFlags & _.COUNTING) != _.COUNTING) {
					return;
				}
				var A = F._mArrowScrollDir;
				if (F._mClickTimer != null && F._mClickTimer.isAlive()) {
					F._mClickTimer.interrupt();
					F.onArrowScroll(A);
				} else {
					F.onPersistArrowScrollEnd(A);
				}
				F._mPrivateFlags &= ~_.COUNTING;
				F._unlisten(document, "mousemove", C);
				F._unlisten(document, "mouseup", D);
			},
			E = function (E) {
				if (F.locked()) {
					return;
				}
				if ((F._mPrivateFlags & _.COUNTING) == _.COUNTING) {
					return;
				}
				var B = this.getAttribute("dir") == "up" ? _.BACKWARD : _.FORWARD;
				F._mArrowScrollDir = B;
				var A = E ? E : window.event;
				F._mCoorX = A.clientX - parseInt(A.offsetX || A.layerX);
				F._mCoorY = A.clientY - parseInt(A.offsetY || A.layerY);
				F._mCoorWidth = this.offsetWidth;
				F._mCoorHeight = this.offsetHeight;
				if (F._mClickTimer == null) {
					F._mClickTimer = new disk.util.TimerService(_.CLICK_TIMEOUT, null);
				} else {
					F._mClickTimer.interrupt();
				}
				F._mClickTimer.setActionListener(function () {
					F.onPersistArrowScrollStart(B);
				});
				F._mClickTimer.start();
				F._mPrivateFlags |= _.COUNTING;
				F._listen(document, "mousemove", C);
				F._listen(document, "mouseup", D);
			};
			if (this._mUI.upArrow) {
				this._mUI.upArrow.setAttribute("dir", "up");
				this._mUI.upArrow.onmousedown = E;
			}
			if (this._mUI.downArrow) {
				this._mUI.downArrow.setAttribute("dir", "down");
				this._mUI.downArrow.onmousedown = E;
			}
			if (this._mUI.thumb && this._mUI.tracker) {
				var B = function (D) {
					if ((F._mPrivateFlags & _.START_DRAGGING) == _.START_DRAGGING) {
						var B = D ? D : window.event,
						C = B.clientX,
						A = B.clientY;
						F._onDragging(C, A);
					}
					if (B.preventDefault) {
						B.preventDefault();
					}
				},
				A = function () {
					if ((F._mPrivateFlags & _.COUNTING) != _.COUNTING) {
						return;
					}
					if ((F._mPrivateFlags & _.START_DRAGGING) == _.START_DRAGGING) {
						F._endDrag();
					}
					F._mPrivateFlags &= ~_.COUNTING;
					F._unlisten(document, "mousemove", B);
					F._unlisten(document, "mouseup", A);
				};
				this._mUI.thumb.onmousedown = function (D) {
					if (F.locked()) {
						return;
					}
					if ((F._mPrivateFlags & _.COUNTING) == _.COUNTING) {
						return;
					}
					var C = D ? D : window.event;
					F._mDX = C.clientX;
					F._mDY = C.clientY;
					F._startDrag();
					F._listen(document, "mouseup", A);
					F._listen(document, "mousemove", B);
					F._mPrivateFlags |= _.COUNTING;
					if (C.preventDefault) {
						C.preventDefault();
					}
				};
			}
		},
		lock : function (A) {
			if (A) {
				this._mPrivateFlags |= _.LOCKED;
				if (this._mUI.scrollbar.className.indexOf("locked") == -1) {
					this._mUI.scrollbar.className += " locked";
				}
			} else {
				this._mPrivateFlags &= ~_.LOCKED;
				this._mUI.scrollbar.className = this._mUI.scrollbar.className.replace(" locked", "");
			}
		},
		locked : function () {
			return (this._mPrivateFlags & _.LOCKED) == _.LOCKED;
		},
		_startDrag : function () {
			this._mPrivateFlags |= _.START_DRAGGING;
		},
		_endDrag : function () {
			this._mPrivateFlags &= ~_.START_DRAGGING;
		},
		onPageScroll : function (_) {},
		inSimulateDragging : function () {
			return (this._mPrivateFlags & _.USING_SIMULATE_DRAG) == _.USING_SIMULATE_DRAG;
		},
		_onDragging : function (C, _) {
			if (this._mDX != -1 && this._mDY != -1) {
				var A = _ - this._mDY,
				B = this._mThumbTop + A;
				B = Math.max(0, B);
				B = Math.min(B, this._mUI.tracker.offsetHeight - this._mUI.thumb.offsetHeight);
				if (!this.inSimulateDragging()) {
					this.setThumbState(B, null, false);
				} else {
					this.onThumbStateChange(B, this._mThumbTop);
				}
			}
			this._mDX = C;
			this._mDY = _;
		},
		_listen : function (_, A, B) {
			if (typeof _.addEventListener != "undefined") {
				_.addEventListener(A, B, false);
			} else {
				if (typeof _.attachEvent != "undefined") {
					_.attachEvent("on" + A, B);
				} else {
					_["on" + A] = B;
				}
			}
		},
		_unlisten : function (_, A, B) {
			if (typeof _.removeEventListener != "undefined") {
				_.removeEventListener(A, B, false);
			} else {
				if (typeof _.detachEvent != "undefined") {
					_.detachEvent("on" + A, B);
				} else {
					_["on" + A] = null;
				}
			}
		},
		abruptPersistArrowScroll : function () {
			this._mPrivateFlags &= ~_.COUNTING;
			if (this._mClickTimer != null && this._mClickTimer.isAlive()) {
				this._mClickTimer.interrupt();
				return true;
			}
			return false;
		},
		onPersistArrowScrollStart : function (_) {},
		onPersistArrowScrollEnd : function (_) {},
		onArrowScroll : function (_) {},
		onScroll : function (A, _) {},
		awakeThumb : function (A) {
			if (A) {
				this._mPrivateFlags |= _.THUMB_AWAKE;
				this._mUI.thumb.style.display = "block";
			} else {
				this._mPrivateFlags &= ~_.THUMB_AWAKE;
				this._mUI.thumb.style.display = "none";
			}
		},
		isThumbAwake : function () {
			return (this._mPrivateFlags & _.THUMB_AWAKE) == _.THUMB_AWAKE;
		},
		setThumbState : function (A, B, D) {
			if (this.locked()) {
				return;
			}
			if (!this.isThumbAwake()) {
				return;
			}
			var C = this._mThumbTop;
			if (B != null) {
				if ((this._mPrivateFlags & _.HAS_BORDER) == _.HAS_BORDER) {
					B -= 1 * 2;
					B = Math.max(0, B);
				}
				try {
					this._mUI.thumb.style.height = B + "px";
				} catch (E) {}

			}
			try {
				this._mUI.thumb.style.top = A + "px";
			} catch (E) {}

			this._mThumbTop = A;
			if (D !== true) {
				this.onThumbStateChange(A, C);
			}
		},
		onThumbStateChange : function (A, _) {},
		getTrackerHeight : function () {
			return this._mUI.tracker.offsetHeight;
		},
		isAwake : function () {
			return (this._mPrivateFlags & _.AWAKE) == _.AWAKE;
		},
		awake : function (A) {
			if (A) {
				this._mPrivateFlags |= _.AWAKE;
				this._mUI.scrollbar.style.display = "block";
			} else {
				this._mPrivateFlags &= ~_.AWAKE;
				this._mUI.scrollbar.style.display = "none";
			}
			this.onAwake(A);
		},
		onAwake : function (_) {},
		reset : function () {
			this.setThumbState(0, 0, false);
		},
		setUsingSimulateDraging : function (A) {
			if (A) {
				this._mPrivateFlags |= _.USING_SIMULATE_DRAG;
			} else {
				this._mPrivateFlags &= ~_.USING_SIMULATE_DRAG;
			}
		},
		setBooleanFlags : function (A, _) {
			if (_) {
				this._mPrivateFlags |= A;
			} else {
				this._mPrivateFlags &= ~A;
			}
		},
		setScrollPosition : function () {
			var B = $("#scrollbar"),
			_ = $(".all-files-headers"),
			A = _.offset().top + _.height() - 1;
			B.css("top", A + "px");
		}
	};
	disk.ui.IScrollbar = _;
})();
(function () {
	var _ = function (_) {
		this._mPrivateFlags = 0;
		this._mView = _;
		this.onWheelChanged = null;
	};
	_.FORWARD = 1;
	_.BACKWARD = 2;
	_.FLAG_PREVENT_DEFAULT = true;
	_.BUILD = 4;
	_.prototype = {
		_init : function () {
			if ((this._mPrivateFlags & _.BUILD) == _.BUILD) {
				return;
			}
			var B = this,
			A = function () {
				var C = window.event ? window.event : arguments[0],
				A = 0;
				if (C.wheelDelta) {
					A = C.wheelDelta / 120;
				} else {
					if (C.detail) {
						A = -C.detail / 3;
					}
				}
				if (A) {
					B._sendWheelChangedMessage(A > 0 ? _.BACKWARD : _.FORWARD, Math.abs(A));
				}
				if (_.FLAG_PREVENT_DEFAULT) {
					if (C.preventDefault) {
						C.preventDefault();
					} else {
						C.returnValue = false;
					}
				}
			};
			if (typeof window.attachEvent != "undefined") {
				this._mView.attachEvent("onmousewheel", A);
			} else {
				if ("onmousewheel" in window) {
					this._mView.addEventListener("mousewheel", A, false);
				} else {
					if (typeof window.addEventListener != "undefined") {
						this._mView.addEventListener("DOMMouseScroll", A, false);
					} else {}

				}
			}
			this._mPrivateFlags |= _.BUILD;
		},
		sense : function () {
			this._init();
		},
		_sendWheelChangedMessage : function (A, _) {
			if (typeof this.onWheelChanged == "function") {
				this.onWheelChanged.call(this, A, _);
			}
		}
	};
	disk.ui.MouseWheelSensor = _;
})();
(function () {
	var _ = function (A, _) {
		this._mPrivateFlags = 0;
		this._mCanvasView = A;
		this._mOrientation = _;
		this._mMotionPoint = [-1, -1];
		this._mLastMotionPoint = [-1, -1];
		this._mNativeSensors = [];
	};
	_.MOTION_LISTENING = 1;
	_.INSTALLED = 2;
	_.MOTION_ACCEPT = 4;
	_.HORIZONTAL = 0;
	_.VERTICAL = 1;
	_.TOUCH_THRESHOLD = 5;
	_.hasMotionCampatibility = function () {
		return "ontouchstart" in document;
	};
	_.prototype = {
		onMotionStart : function (A, _) {},
		onMotionEnd : function (A, _) {},
		onMotion : function (D, B, _, A, C) {},
		dispatchMotionStart : function (B, A) {
			this._mMotionPoint[0] = B;
			this._mMotionPoint[1] = A;
			if (disk.DEBUG) {
				console.log("Motion Start " + B + ":" + A);
			}
			this._mPrivateFlags |= _.MOTION_LISTENING;
			this.onMotionStart(this._mCanvasView, this._mMotionPoint);
		},
		dispatchMotionMove : function (J, G) {
			var C = _.MOTION_LISTENING,
			I = _.MOTION_ACCEPT;
			if ((this._mPrivateFlags & C) != C) {
				return;
			}
			var H = this._mMotionPoint[0],
			E = this._mMotionPoint[1],
			B = this._mLastMotionPoint[0] == -1 ? H : this._mLastMotionPoint[0],
			F = this._mLastMotionPoint[1] == -1 ? E : this._mLastMotionPoint[1];
			this._mLastMotionPoint[0] = J;
			this._mLastMotionPoint[1] = G;
			var A = J - B,
			D = G - F;
			if (disk.DEBUG) {
				console.log("Motion Delta: " + A + "-" + D);
			}
			if ((this._mPrivateFlags & I) == I) {
				this.onMotion(this._mCanvasView, this._mLastMotionPoint, A, D, this._mOrientation);
				return;
			}
			if (this._motionAccept(H, E, J, G)) {
				if (disk.DEBUG) {
					console.log("touch accept");
				}
				this._mPrivateFlags |= _.MOTION_ACCEPT;
				this.onMotion(this._mCanvasView, this._mLastMotionPoint, A, D, this._mOrientation);
			}
		},
		dispatchMotionEnd : function () {
			if (disk.DEBUG) {
				console.log("Touch End");
			}
			var A = _.MOTION_LISTENING;
			if ((this._mPrivateFlags & A) != A) {
				return;
			}
			this._mPrivateFlags &= ~(_.MOTION_LISTENING | _.MOTION_ACCEPT);
			this._mMotionPoint[0] = -1;
			this._mMotionPoint[1] = -1;
			this._mLastMotionPoint[0] = -1;
			this._mLastMotionPoint[1] = -1;
		},
		getOrientation : function () {
			return this._mOrientation;
		},
		setOrientation : function (_) {
			this._mOrientation = _;
		},
		getInitialMotionPoint : function () {
			return this._mInitialPoint;
		},
		_motionAccept : function (B, G, F, D) {
			var A = F - B,
			C = D - G,
			H = Math.abs(A),
			E = Math.abs(C);
			if (this._mOrientation == _.HORIZONTAL) {
				if (E > H) {
					return false;
				}
				if (H < _.TOUCH_THRESHOLD) {
					return false;
				}
				return true;
			} else {
				if (H > E) {
					return false;
				}
				if (E < _.TOUCH_THRESHOLD) {
					return false;
				}
				return true;
			}
		},
		uninstall : function () {
			var A = _.INSTALLED;
			if ((this._mPrivateFlags & A) != A) {
				return;
			}
			this._mPrivateFlags &= ~(_.MOTION_LISTENING | _.MOTION_ACCEPT | _.INSTALLED);
			this._mCanvasView.removeEventListener("touchstart", this._mNativeSensors[0], false);
			this._mCanvasView.removeEventListener("touchmove", this._mNativeSensors[1], false);
			document.removeEventListener("touchend", this._mNativeSensors[2], false);
			document.removeEventListener("touchcancel", this._mNativeSensors[2], false);
		},
		install : function () {
			var B = _.INSTALLED,
			E = this;
			if ((this._mPrivateFlags & B) != B) {
				this._mPrivateFlags |= B;
				var C = function (_) {
					var A = _.touches[0];
					E.dispatchMotionStart(A.clientX, A.clientY);
					return false;
				};
				this._mCanvasView.addEventListener("touchstart", C, false);
				this._mNativeSensors[0] = C;
				var D = function (_) {
					var A = _.touches[0];
					E.dispatchMotionMove(A.clientX, A.clientY);
					_.preventDefault();
					_.stopPropagation();
					return false;
				};
				this._mCanvasView.addEventListener("touchmove", D, false);
				this._mNativeSensors[1] = D;
				var A = function (_) {
					E.dispatchMotionEnd();
				};
				document.addEventListener("touchend", A, false);
				document.addEventListener("touchcancel", A, false);
				this._mNativeSensors[2] = A;
			}
		}
	};
	disk.ui.MotionSensor = _;
})();
(function () {
	var _ = function () {
		this._mPrivateFlags = 0;
	};
	_.prototype = {
		onMount : function (_) {},
		onConnectivity : function (_) {},
		onKeyAction : function (_) {},
		onKeyEvent : function (_, A) {}

	};
	disk.ui.KeyguardListener = _;
	var A = function () {
		this._mPrivateFlags = 0;
		this._mListeners = [];
		this._mHijacker = null;
		this._mHijackContext = null;
	};
	A.EVENT_ARROW_UP = 38;
	A.EVENT_ARROW_DOWN = 40;
	A.EVENT_ARROW_LEFT = 37;
	A.EVENT_ARROW_RIGHT = 39;
	A.EVENT_PAGE_UP = 33;
	A.EVENT_PAGE_DOWN = 34;
	A.EVENT_ENTER = 13;
	A.EVENT_ESCAPE = 27;
	A.INSTALL = 1;
	A.EXCLUSIVE_LOCK = 2;
	A.prototype = {
		acquire : function (D, C) {
			if ((this._mPrivateFlags & A.EXCLUSIVE_LOCK) == A.EXCLUSIVE_LOCK) {
				throw new Error("exclusive lock must be relase before anybody else try to acqiure keyguard service");
			}
			if (disk.DEBUG) {
				console.log("execusive acquire ", C);
			}
			this._install();
			if (C) {
				this._mPrivateFlags |= A.EXCLUSIVE_LOCK;
				for (var B = 0, _ = this._mListeners.length; B < _; B++) {
					this._mListeners[B].onConnectivity(false);
				}
			}
			this._mListeners.push(D);
			D.onMount(true);
		},
		release : function (D) {
			var C = (this._mPrivateFlags & A.EXCLUSIVE_LOCK) == A.EXCLUSIVE_LOCK,
			E = false;
			for (var B = 0, _ = this._mListeners.length; B < _; B++) {
				if (D == this._mListeners[B]) {
					this._mListeners[B].onMount(false);
					this._mListeners.splice(B, 1);
					if (!C) {
						break;
					}
				} else {
					if (E) {
						this._mListners[B].onConnectivity(true);
					}
				}
			}
			if (C) {
				this._mPrivateFlags &= ~A.EXCLUSIVE_LOCK;
			}
			this._mHijacker = null;
			this._mHijackContext = null;
			if (disk.DEBUG) {
				console.log("relase keyguard and hijacker");
			}
			if (this._mListeners.length == 0) {
				this._uninstall();
			}
		},
		hijackKeyEvent : function (_, B) {
			var C = (this._mPrivateFlags & A.EXCLUSIVE_LOCK) == A.EXCLUSIVE_LOCK;
			if (C) {
				this._mHijacker = _;
				this._mHijackContext = B;
				if (disk.DEBUG) {
					console.log("hijack keyevent now");
				}
				return true;
			}
			return false;
		},
		dispatchKeyEvent : function (D) {
			var E = (this._mPrivateFlags & A.EXCLUSIVE_LOCK) == A.EXCLUSIVE_LOCK,
			F = null;
			for (var B = this._mListeners.length, C = B - 1; C >= 0; C--) {
				F = this._mListeners[C];
				var G = true;
				G = G & D.shiftKey;
				G = G & D.ctrlKey;
				G = G & D.altKey;
				var _ = false;
				if (!G) {
					switch (D.keyCode) {
					case A.EVENT_ARROW_UP:
					case A.EVENT_ARROW_DOWN:
					case A.EVENT_ARROW_LEFT:
					case A.EVENT_ARROW_RIGHT:
					case A.EVENT_PAGE_UP:
					case A.EVENT_PAGE_DOWN:
					case A.EVENT_ENTER:
					case A.EVENT_ESCAPE:
						_ = F.onKeyAction(D.keyCode);
						break;
					default:
					}
				}
				if (!_) {
					if (this._mHijacker) {
						this._mHijacker.call(this._mHijackContext ? this._mHijackContext : this, D.keyCode, {
							shift : D.shiftKey,
							ctrl : D.ctrlKey,
							alt : D.altKey
						});
					} else {
						F.onKeyEvent(D.keyCode, {
							shift : D.shiftKey,
							ctrl : D.ctrlKey,
							alt : D.altKey
						});
					}
				}
				if (C == B - 1 && E) {
					break;
				}
			}
		},
		_install : function () {
			if ((this._mPrivateFlags & A.INSTALL) != A.INSTALL) {
				var _ = this;
				$(document).bind("keyup", function (A) {
					_.dispatchKeyEvent(A);
				});
				if (disk.DEBUG) {
					console.log("Keyguard service installed");
				}
				this._mPrivateFlags |= A.INSTALL;
			}
		},
		_uninstall : function () {
			if ((this._mPrivateFlags & A.INSTALL) == A.INSTALL) {
				$(document).unbind("keyup");
				if (disk.DEBUG) {
					console.log("Keyguard service uninstalled");
				}
				this._mPrivateFlags &= ~A.INSTALL;
			}
		}
	};
	disk.ui.Keyguard = A;
})();
(function () {
	var _ = function () {
		this._mPopupPool = [];
		this._mCurrentPopup = null;
	};
	_.PRIORITY_HIGH = 0;
	_.PRIORITY_LOW = 1;
	_._sTag = 0;
	_.obtainUniqueTag = function () {
		return (1 << 10) + (_._sTag++);
	};
	_.prototype = {
		schedule : function (C, D, E) {
			if (C instanceof disk.ui.Panel) {
				var F = this,
				B = C.onVisibilityChange;
				C.onVisibilityChange = function (_) {
					F._dispatchPopupShutdown(this, _, this.tagHideBehind);
				};
				C.tagHideBehind = D;
				var A = {
					popup : C,
					tag : D,
					callback : B
				};
				if (E == _.PRIORITY_HIGH) {
					this._mPopupPool.unshift(A);
				} else {
					this._mPopupPool.push(A);
				}
				if (disk.DEBUG) {
					console.log("Sign in promotion ", D);
				}
			} else {
				if (disk.DEBUG) {
					console.warn("promotion component is not considered as a illegal disk.ui.Panel");
				}
			}
		},
		unschedule : function (B) {
			for (var A = 0, _ = this._mPopupPool.length; A < _; A++) {
				if (this._mPopupPool[A].tag == B) {
					this._mPopupPool[A].popup.onVisibilityChange = this._mPopupPool[A].callback;
					this._mPopupPool.splice(A, 1);
					break;
				}
			}
		},
		_dispatchPopupShutdown : function (D, C, E) {
			var A = false;
			for (var B = 0, _ = this._mPopupPool.length; B < _; B++) {
				if (E == this._mPopupPool[B].tag) {
					A = true;
					this._mPopupPool[B].callback.call(D, C);
					if (!C) {
						this._mPopupPool[B].popup.onVisibilityChange = this._mPopupPool[B].callback;
						this._mPopupPool.splice(B, 1);
						this._iterate();
					}
					break;
				}
			}
			if (!A) {
				if (disk.DEBUG) {
					console.warn("can not resolve an registered popup to dispatch visibility event");
				}
			}
		},
		_iterate : function () {
			if (this._mPopupPool.length > 0) {
				var _ = this._mPopupPool[0];
				_.popup.setVisible(true);
				this._mCurrentPopup = _;
			} else {
				if (disk.DEBUG) {
					console.log("no promotion eligible found, service should shutdown");
				}
			}
		},
		start : function () {
			if (disk.DEBUG) {
				console.log("Promotion Service start normally");
			}
			this._iterate();
		},
		stop : function () {
			if (disk.DEBUG) {
				console.log("Promotion Service stop normally");
			}
			this._mPopupPool.length = 0;
			if (this._mCurrentPopup != null) {
				this._mCurrentPopup.popup.setVisible(false);
			}
		}
	};
	disk.ui.PromotionService = _;
})();
(function () {
	var _ = function (_) {
		this._mHost = _;
		this._mDOM = null;
		this._mEditing = false;
		this._mEmptyNode = false;
		this.build();
	};
	_.DEEP_UNIT = 15;
	_.activeLeaf = null;
	_.prototype = {
		build : function () {
			var B = document.createElement("div"),
			C = this;
			B.href = "javascript:;";
			B.className = "treeview-node";
			if (this._mHost._mConfig.isParti === true && !disk.ui.Utilities.IGNORE_PARTICULAR_DIR) {
				B.className += " treeview-node-parti";
			}
			B.hideFocus = "true";
			B.onmouseover = function () {
				if (C._mHost._mConfig.isParti) {
					return false;
				}
				$(this).addClass("treeview-node-hover");
				return false;
			};
			B.onmouseout = function () {
				if (C._mHost._mConfig.isParti) {
					return false;
				}
				$(this).removeClass("treeview-node-hover");
				return false;
			};
			if (this._mHost._mConfig.css) {
				B.className += " " + this._mHost._mConfig.css;
			}
			B.style.paddingLeft = _.DEEP_UNIT * (this._mHost.getDeep() - 1) + "px";
			B.setAttribute("_pl", B.style.paddingLeft);
			var A = "";
			if (this._mHost._mConfig.isParti === true && !disk.ui.Utilities.IGNORE_PARTICULAR_DIR) {
				A = " treeview-ic-parti";
			}
			B.innerHTML = '<span class="treeview-node-handler"><em class="b-in-blk plus sprite-ic2"></em><dfn class="b-in-blk treeview-ic' + A + '"></dfn><span class="treeview-txt">' + disk.ui.Utilities.encodeHTML(this._mHost._mConfig.text) + "</span></span>";
			this._mDOM = B;
			$(this._mDOM).click(function (_) {
				if (C._mHost._mConfig.isParti && !disk.ui.Utilities.IGNORE_PARTICULAR_DIR) {
					return false;
				}
				if (C._mEditing) {
					return false;
				}
				if (_.target.nodeName == "EM") {
					if (C._mEmptyNode) {
						if (C._mHost._mConfig.onSelect) {
							C._mHost._mConfig.onSelect.call(C, C._mHost, null, false, true);
						}
						return;
					}
					if (C._mHost.children().length > 0) {
						C._mHost.toggle();
					} else {
						if (!C._mHost.expanded()) {
							if (C._mHost._mConfig.onSelect) {
								C._mHost._mConfig.onSelect.call(C, C._mHost, null, true);
							}
							C._mHost.expand(true);
						} else {
							C._mHost.expand(false);
						}
					}
				} else {
					C.requestFocus();
				}
				return false;
			});
		},
		requestFocus : function () {
			var A = this;
			if (_.activeLeaf != A && _.activeLeaf != null) {
				_.activeLeaf.setSelected(false);
			}
			if (!A.selected()) {
				A.setSelected(true);
				if (!A._mEmptyNode) {
					A._mHost.expand(true);
					if (A._mHost._mConfig.onSelect) {
						A._mHost._mConfig.onSelect.call(A, A._mHost);
					}
				} else {
					if (A._mHost._mConfig.onSelect) {
						A._mHost._mConfig.onSelect.call(A, A._mHost, null, false, true);
					}
				}
			} else {
				if (!A._mEmptyNode) {
					A._mHost.toggle();
				} else {
					if (A._mHost._mConfig.onSelect) {
						A._mHost._mConfig.onSelect.call(A, A._mHost, null, false, true);
					}
				}
			}
			_.activeLeaf = A;
		},
		setAsEmpty : function (_) {
			if (_) {
				$(this._mDOM).addClass("treenode-empty");
			} else {
				$(this._mDOM).removeClass("treenode-empty");
			}
			this._mEmptyNode = _;
		},
		selected : function () {
			return $(this._mDOM).hasClass("treeview-node-on");
		},
		setSelected : function (_) {
			if (this._mDOM) {
				if (_) {
					$(this._mDOM).addClass("treeview-node-on");
				} else {
					$(this._mDOM).removeClass("treeview-node-on");
				}
			}
		},
		setLoading : function (_) {
			if (_) {
				$(".treeview-ic", this._mDOM).addClass("treeview-leaf-loading");
			} else {
				$(".treeview-ic", this._mDOM).removeClass("treeview-leaf-loading");
			}
		},
		setIconIndicator : function (_) {
			if (_) {
				$("em", this._mDOM).addClass("minus");
				$(this._mDOM).addClass("_minus");
			} else {
				$("em", this._mDOM).removeClass("minus");
				$(this._mDOM).removeClass("_minus");
			}
		},
		getHost : function () {
			return this._mHost;
		},
		getNode : function () {
			return this._mDOM;
		},
		setText : function (A) {
			var _ = document.createTextNode(A),
			B = $(".treeview-node-handler", this._mDOM)[0],
			C = B.childNodes[B.childNodes.length - 1];
			if (C.nodeType == 3) {
				B.replaceChild(_, C);
			} else {
				B.appendChild(_);
			}
		},
		edit : function (D, _) {
			if (D) {
				var A = this._mHost._mConfig.text,
				B = $(".treeview-node-handler", this._mDOM)[0];
				if (A.trim().length != 0) {
					var C = B.childNodes[B.childNodes.length - 1];
					B.removeChild(C);
				}
				if (typeof _ == "function") {
					_.call(this, true, B);
				}
			} else {
				if (typeof _ == "function") {
					_.call(this, false, B);
				}
			}
			this._mEditing = D;
		}
	};
	disk.ui.TreeLeaf = _;
	var A = function (A, _) {
		this._mLeaf = null;
		this._mParent = A;
		this._mConfig = _;
		this._mChildren = [];
		this._mDOM = null;
		this._mAttrs = {};
		this.build();
	};
	A.prototype = {
		build : function () {
			var A = document.createElement("ul");
			A.className = "treeview treeview-content treeview-collapse";
			if (this._mConfig.treeContentCss) {
				A.className += " " + this._mConfig.treeContentCss;
			}
			if (!this.isRoot()) {
				var B = document.createElement("li");
				this._mLeaf = new _(this);
				this._mLeaf.setAsEmpty(this._mConfig.dir_empty == 1);
				B.appendChild(this._mLeaf.getNode());
				B.appendChild(A);
				this._mParent.getNode().appendChild(B);
				this._mParent._mChildren.push(this);
			} else {
				this._mConfig.host.appendChild(A);
			}
			this._mDOM = A;
		},
		remove : function () {
			for (var B = 0, A = this._mParent._mChildren, _ = A.length; B < _; B++) {
				if (A[B] == this) {
					A.splice(B, 1);
					var C = this.isRoot() ? this._mDOM : this._mDOM.parentNode;
					C.parentNode.removeChild(C);
					break;
				}
			}
		},
		children : function () {
			return this._mChildren;
		},
		setLoading : function (_) {
			this._mLeaf.setLoading(_);
		},
		getLeaf : function () {
			return this._mLeaf;
		},
		toggle : function () {
			this.expand(!this.expanded());
		},
		expand : function (_) {
			if (_) {
				$(this._mDOM).removeClass("treeview-collapse");
			} else {
				$(this._mDOM).addClass("treeview-collapse");
			}
			if (typeof this._mConfig.onExpand == "function") {
				this._mConfig.onExpand.call(this, _, this);
			}
			if (this._mLeaf) {
				this._mLeaf.setIconIndicator(this.expanded());
			}
		},
		expanded : function () {
			if (this.isRoot()) {
				return true;
			}
			return !$(this._mDOM).hasClass("treeview-collapse");
		},
		getDeep : function () {
			return this.isRoot() ? 0 : this._mParent.getDeep() + 1;
		},
		isRoot : function () {
			return !!this._mConfig.root;
		},
		setAttr : function (A, _) {
			this._mAttrs[A] = _;
		},
		getAttr : function (_) {
			return this._mAttrs[_];
		},
		getNode : function () {
			return this._mDOM;
		},
		getParent : function () {
			return this._mParent;
		}
	};
	disk.ui.TreeNode = A;
})();
(function () {
	var A = function () {
		this._mRecycledViews = [];
		this._mActiveViews = [];
		this._mFirstPosition = 0;
	};
	A.prototype = {
		add : function (_) {
			this._mRecycledViews.push(_);
			_.parentNode.removeChild(_);
		},
		get : function (A) {
			if (this._mRecycledViews.length == 0) {
				return null;
			}
			var _ = this._mRecycledViews.pop();
			return _;
		},
		scrapActiveViews : function () {
			var C = this._mActiveViews,
			D = C.length;
			if (D == 0) {
				return;
			}
			for (var B = 0, A = C.length; B < A; B++) {
				this._mRecycledViews.push(C[B]);
			}
			while (this._mRecycledViews.length >= D) {
				var _ = this._mRecycledViews.pop();
				if (_) {
					_.parentNode.removeChild(_);
					_ = null;
				}
			}
		},
		getActiveView : function (A) {
			var C = A - this._mFirstPosition,
			B = this._mActiveViews;
			if (C < 0 || C > B.length - 1) {
				return null;
			}
			var _ = B[C];
			B[C] = null;
			return _;
		},
		preseveActiveViews : function (D, B) {
			var C = this._mActiveViews;
			C.length = D.length;
			for (var A = 0, _ = D.length; A < _; A++) {
				C[A] = D[A];
			}
			this._mFirstPosition = B;
		},
		clear : function () {
			this._mRecycledViews.length = 0;
		}
	};
	disk.ui.ViewRecycler = A;
	var _ = function (B, A) {
		this._mUI = B;
		this._mConfig = A || {};
		this._mPrivateFlags = 0;
		this._mFirstPosition = 0;
		this._mChildrenMarginTop = 0;
		this._mChildrenCount = 0;
		this._mElementsData = null;
		this._mItemCount = 0;
		this._mItemHeight = 0;
		this._mWheelSensor = null;
		this._mViewRecycler = null;
		this._mMotionSensor = null;
		this._mChildren = [];
		this._mSmoothScroller = null;
		this._mSmoothScrollDelta = 0;
		this._mSmoothScrollRemaining = 0;
		this._mScrollDir = -1;
		this._mLayoutMode = _.LAYOUT_MODE_NORMAL;
		this._mSpecificPosition = -1;
		this._mIScrollbar = null;
		this._mPersistScrollDir = -1;
		this._mCheckedChildren = [];
		this._mSyncTop = 0;
		this._mSyncPosition = -1;
		this._mSyncId = null;
		this._mDebugAlias = null;
	};
	_.CLONE_VIEW_BUILD = 1;
	_.DATA_CHANGED = 2;
	_.BUILD = 4;
	_.SMOOTH_SCROLLING = 8;
	_.USING_SCROLLBAR = 16;
	_.USING_KEYBOARD_DISPATCHER = 32;
	_.USING_MOUSE_WHEEL_SENSOR = 64;
	_.CHECKED_ALL = 128;
	_.PRESERVE_CHECKED_STATE = 256;
	_.LOCKED = 512;
	_.USING_LOW_PIXEL_RATIO = 1024;
	_.USING_TOUCH_SENSOR = 2048;
	_.TOP = 0;
	_.BOTTOM = 1;
	_.NOTIFY_LIST_EMPTY = 0;
	_.NOTIFY_LIST_REPAINT = 1;
	_.NOTIFY_SYSTEM_LOCK = 2;
	_.WHEEL_TO_PIXEL_RATIO = 120 / 2;
	_.WHEEL_TO_PIXEL_RATIO_LOW = 120 / 6;
	_.SMOOTH_SCROLL_DURATION = 150;
	_.SMOOTH_SCROLL_INTERVAL = 5;
	_.PIXEL_RATIO_ON_DRAGGING_SCALE = 1;
	_.LAYOUT_MODE_NORMAL = 0;
	_.LAYOUT_MODE_FORCE_TOP = 1;
	_.LAYOUT_MODE_FORCE_BOTTOM = 2;
	_.LAYOUT_MODE_SPECIFIC = 3;
	_.LAYOUT_MODE_FROM_SPECIFIC = 4;
	_.prototype = {
		getView : function (A, B, _) {
			return null;
		},
		deactivate : function (A) {
			if (A) {
				this._mUI.listContainer.style.display = "none";
				this.onSystemNotify(_.NOTIFY_LIST_EMPTY, true, true);
			} else {
				this._mUI.listContainer.style.display = "block";
				this.onSystemNotify(_.NOTIFY_LIST_EMPTY, false, true);
			}
		},
		setBackedData : function (A) {
			if (this.locked()) {
				return;
			}
			if ((this._mPrivateFlags & _.BUILD) != _.BUILD) {
				this._build();
				this._mPrivateFlags |= _.BUILD;
			}
			this._resetList();
			this._mElementsData = A;
			this._mItemCount = A == null ? 0 : A.length;
			this._layout();
		},
		setBooleanFlag : function (A, _) {
			if (_) {
				this._mPrivateFlags |= A;
			} else {
				this._mPrivateFlags &= ~A;
			}
		},
		setUsingScrollbar : function (A) {
			if (A) {
				this._mPrivateFlags |= _.USING_SCROLLBAR;
			} else {
				this._mPrivateFlags &= ~_.USING_SCROLLBAR;
			}
		},
		isUsingScrollbar : function () {
			return (this._mPrivateFlags & _.USING_SCROLLBAR) == _.USING_SCROLLBAR;
		},
		requestLayout : function () {
			if (this.locked()) {
				return;
			}
			this._mPrivateFlags |= _.DATA_CHANGED;
			this._mLayoutMode = _.LAYOUT_MODE_SPECIFIC;
			this._mPrivateFlags |= _.PRESERVE_CHECKED_STATE;
			this._layout();
			this._mPrivateFlags &= ~_.PRESERVE_CHECKED_STATE;
		},
		appendBackedData : function (B) {
			if (this._mElementsData == null || this._mItemCount == 0) {
				this.setBackedData(B);
				return;
			}
			if (this.locked()) {
				return;
			}
			for (var A = 0, _ = B.length; A < _; A++) {
				this._mElementsData.push(B[A]);
			}
			this._mItemCount = this._mElementsData.length;
			this.requestLayout();
		},
		updateBackedData : function (C, A) {
			if (this.locked()) {
				return;
			}
			this._mElementsData = C;
			this._mItemCount = C == null ? 0 : C.length;
			this._mPrivateFlags |= _.DATA_CHANGED;
			this._mLayoutMode = _.LAYOUT_MODE_SPECIFIC;
			if (this._mSyncId != null) {
				var B = this.lookupPositionForId(this._mSyncId);
				if (B != -1) {
					if (disk && disk.DEBUG) {
						console.log("recover a position at ", B);
					}
					this._mSyncPosition = B;
				}
			}
			if (A === true) {
				this._mPrivateFlags |= _.PRESERVE_CHECKED_STATE;
			} else {
				this._mCheckedChildren.length = 0;
			}
			this._layout();
			if (A === true) {
				this._mPrivateFlags &= ~_.PRESERVE_CHECKED_STATE;
			}
		},
		getIdForPosition : function (_) {
			return null;
		},
		lookupPositionForId : function (_) {
			return -1;
		},
		changeBackedData : function (A, B) {
			if (this.locked()) {
				return;
			}
			if (B === true) {
				this._resetList();
			}
			this._mElementsData = A;
			this._mItemCount = A == null ? 0 : A.length;
			this._mPrivateFlags |= _.DATA_CHANGED;
			this._mLayoutMode = _.LAYOUT_MODE_FORCE_TOP;
			this._layout();
		},
		dispatchDataChanged : function () {
			this._mPrivateFlags |= _.DATA_CHANGED;
		},
		getScrollTop : function () {
			return this._mFirstPosition * this._mItemHeight + Math.abs(this._mChildrenMarginTop);
		},
		getFirstCheckedChild : function () {
			if (this.isAllItemChecked()) {
				return this._mChildren[this._mFirstPosition - this._mFirstPosition];
			}
			for (var A = 0, _ = this._mCheckedChildren.length; A < _; A++) {
				if (this._mCheckedChildren[A] === true) {
					return this._mChildren[A - this._mFirstPosition];
				}
			}
			return null;
		},
		getRenderingChildAt : function (_) {
			if (this._mChildren.length == 0) {
				return null;
			}
			if (_ < 0 || _ > this._mChildren.length - 1) {
				return null;
			}
			return this._mChildren[_];
		},
		getRenderingChildByPosition : function (_) {
			var A = _ - this._mFirstPosition;
			return this.getRenderingChildAt(A);
		},
		getScrollbar : function (B) {
			var C = this,
			A = this._mIScrollbar;
			if (A == null && B === true) {
				A = new disk.ui.IScrollbar(this._mUI);
				A.onArrowScroll = function (_) {
					C.arrowScroll(_);
					C._mPersistScrollDir = -1;
				};
				A.onPersistArrowScrollStart = function (_) {
					C._mPersistScrollDir = _;
					C.arrowScroll(_);
				};
				A.onPersistArrowScrollEnd = function (_) {
					C._mPersistScrollDir = -1;
				};
				A.onPageScroll = function (_) {
					C._mPersistScrollDir = -1;
					C.pageScroll(_);
				};
				A.setUsingSimulateDraging(true);
				A.onThumbStateChange = function (F, E) {
					var D = _.PIXEL_RATIO_ON_DRAGGING_SCALE * Math.abs(F - E),
					B = D * C._mItemHeight * C._mItemCount / A.getTrackerHeight(),
					G = F - E > 0 ? disk.ui.MouseWheelSensor.FORWARD : disk.ui.MouseWheelSensor.BACKWARD;
					C.scrollBy(G, B);
				};
				this._mIScrollbar = A;
			}
			return A;
		},
		onComputeScrollbarState : function (E, C, _) {
			var B = this._mUI.listContainer.parentNode.offsetHeight,
			D = this._mItemHeight,
			F = E.getTrackerHeight(),
			G = (this._mItemCount == 0) ? 0 : (F * B) / (D * this._mItemCount),
			A = (this._mItemCount == 0) ? 0 : F * this.getScrollTop() / (D * this._mItemCount);
			A = Math.min(A, F - G);
			A = Math.max(A, 0);
			E.setThumbState(A, G, true);
		},
		awakeScrollbar : function (A) {
			if (A) {
				var _ = this.getScrollbar(true);
				if (!_.isAwake()) {
					_.awake(true);
				}
			} else {
				_ = this.getScrollbar();
				if (_ != null && _.isAwake()) {
					_.awake(false);
				}
			}
		},
		scrollToPosition : function (B) {
			if (this.locked()) {
				return;
			}
			if (B < 0 || B > this._mItemCount - 1) {
				return false;
			}
			var A = B > this._mFirstPosition;
			this._mPrivateFlags |= _.DATA_CHANGED;
			this._mLayoutMode = _.LAYOUT_MODE_FROM_SPECIFIC;
			this._mSpecificPosition = B;
			this._mChildrenMarginTop = 0;
			this._mUI.listContainer.style.marginTop = "0";
			this._mPrivateFlags |= _.PRESERVE_CHECKED_STATE;
			this._layout();
			this._mPrivateFlags &= ~_.PRESERVE_CHECKED_STATE;
			if (A) {
				this._fixTooHigh();
			} else {
				this._fixTooLow();
			}
			return true;
		},
		pageScroll : function (B) {
			if (this.locked()) {
				return;
			}
			var A = this._mFirstPosition,
			_ = -1;
			if (B == disk.ui.MouseWheelSensor.FORWARD) {
				_ = A + this._mChildren.length;
				_ = Math.min(_, this._mItemCount - 1);
			} else {
				_ = A - this._mChildren.length;
				_ = Math.max(_, 1);
			}
			this.scrollToPosition(_);
		},
		arrowScroll : function (B) {
			if (this.locked()) {
				return;
			}
			var A = _.WHEEL_TO_PIXEL_RATIO;
			if ((this._mPrivateFlags & _.USING_LOW_PIXEL_RATIO) == _.USING_LOW_PIXEL_RATIO) {
				A = _.WHEEL_TO_PIXEL_RATIO_LOW;
			}
			this.smoothScroll(B, 1 * A);
		},
		_resetList : function () {
			this._mChildren.length = 0;
			this._mChildrenMarginTop = 0;
			this._mUI.listContainer.style.marginTop = "0";
			this._mFirstPosition = 0;
			this._mElementsData = null;
			this._mItemCount = 0;
			this._mSyncTop = 0;
			this._mSyncPosition = -1;
			this._mSyncId = null;
			this._mPrivateFlags &= ~_.CLONE_VIEW_BUILD;
			this._mPrivateFlags &= ~_.DATA_CHANGED;
			this._mViewRecycler.clear();
			if (this._mUI.listContainer.nodeName.toUpperCase() == "TABLE") {
				for (var B = this._mUI.listContainer.rows, A = B.length, C = A - 1; C >= 0; C--) {
					this._mUI.listContainer.deleteRow(C);
				}
			} else {
				this._mUI.listContainer.innerHTML = "";
			}
		},
		getBackedData : function () {
			return this._mElementsData;
		},
		_handleDataChanged : function () {
			if ((this._mPrivateFlags & _.PRESERVE_CHECKED_STATE) != _.PRESERVE_CHECKED_STATE) {
				this._mCheckedChildren.length = 0;
				this._mPrivateFlags &= ~_.CHECKED_ALL;
			}
		},
		setItemChecked : function (C, D) {
			this._mCheckedChildren[C] = D;
			var E = (this._mPrivateFlags & _.CHECKED_ALL) == _.CHECKED_ALL;
			if (!D && E) {
				this._mPrivateFlags &= ~_.CHECKED_ALL;
				for (var B = 0, A = this._mElementsData.length; B < A; B++) {
					if (B != C) {
						this._mCheckedChildren[B] = true;
					}
				}
			}
		},
		isItemChecked : function (A) {
			var B = (this._mPrivateFlags & _.CHECKED_ALL) == _.CHECKED_ALL;
			if (B) {
				return true;
			}
			return this._mCheckedChildren[A] === true;
		},
		setItemsChecked : function (A) {
			if (A) {
				this._mPrivateFlags |= _.CHECKED_ALL;
			} else {
				this._mPrivateFlags &= ~_.CHECKED_ALL;
				this._mCheckedChildren.length = 0;
			}
		},
		lock : function (B, C) {
			var A = this.getScrollbar();
			if (B) {
				this._mPrivateFlags |= _.LOCKED;
				if (A != null) {
					A.lock(true);
				}
			} else {
				this._mPrivateFlags &= ~_.LOCKED;
				if (A != null) {
					A.lock(false);
				}
			}
			if (C !== true) {
				this.onSystemNotify(disk.ui.RecycleListView.NOTIFY_SYSTEM_LOCK, B);
			}
		},
		onSystemNotify : function (_, A) {},
		locked : function () {
			return (this._mPrivateFlags & _.LOCKED) == _.LOCKED;
		},
		isAllItemChecked : function () {
			return (this._mPrivateFlags & _.CHECKED_ALL) == _.CHECKED_ALL;
		},
		getCheckedItems : function () {
			if (this.isAllItemChecked()) {
				if (this._mElementsData == null) {
					return [];
				} else {
					var B = [];
					for (var A = 0, _ = this._mElementsData.length; A < _; A++) {
						B.push(this._mElementsData[A]);
					}
					return B;
				}
			}
			B = [];
			if (!this._mElementsData) {
				return B;
			}
			for (A = 0, _ = this._mCheckedChildren.length; A < _; A++) {
				if (this._mCheckedChildren[A] === true) {
					B.push(this._mElementsData[A]);
				}
			}
			return B;
		},
		getElementsData : function () {
			return this._mElementsData;
		},
		getFirstCheckedIndex : function () {
			if (this.isAllItemChecked()) {
				return 0;
			}
			for (var A = 0, _ = this._mCheckedChildren.length; A < _; A++) {
				if (this._mCheckedChildren[A] === true) {
					return A;
				}
			}
			return -1;
		},
		hitInVisibleRegion : function (_) {
			return _ >= this._mFirstPosition && _ < this._mFirstPosition + this._mChildren.length;
		},
		_correctSyncPosition : function () {
			if (this._mElementsData.length > 0 && this._mSyncPosition > this._mElementsData.length - 1) {
				this._mSyncPosition = -1;
			}
		},
		_layout : function () {
			if (this._mItemCount == 0) {
				this._resetList();
				this.onPositionChanged(this._mFirstPosition, this._mChildren.length);
				this._turnPluginDriverOnOrOff();
				this.onSystemNotify(disk.ui.RecycleListView.NOTIFY_LIST_EMPTY, true);
				this.onSystemNotify(disk.ui.RecycleListView.NOTIFY_LIST_REPAINT, true);
				return;
			} else {
				this.onSystemNotify(disk.ui.RecycleListView.NOTIFY_LIST_EMPTY, false);
				this.onSystemNotify(disk.ui.RecycleListView.NOTIFY_LIST_REPAINT, true);
			}
			if ((this._mPrivateFlags & _.DATA_CHANGED) == _.DATA_CHANGED) {
				this._handleDataChanged();
			}
			if ((this._mPrivateFlags & _.DATA_CHANGED) == _.DATA_CHANGED) {
				for (var A = this._mChildren.length - 1; A >= 0; A--) {
					this._mViewRecycler.add(this._mChildren.pop());
				}
			} else {
				for (A = this._mChildren.length - 1; A >= 0; A--) {
					this._mViewRecycler.addActiveView(this._mChildren.pop());
				}
			}
			this._mUI.listContainer.style.marginTop = "0";
			this._mChildrenMarginTop = 0;
			switch (this._mLayoutMode) {
			case _.LAYOUT_MODE_FROM_SPECIFIC:
				var B = this._mSpecificPosition - 1;
				this._mFirstPosition = B;
				this._fillFromTop(B);
				this._mSyncTop = 0;
				this._mSyncPosition = B;
				this._mSyncId = null;
				break;
			case _.LAYOUT_MODE_SPECIFIC:
				this._correctSyncPosition();
				this._mFirstPosition = this._mSyncPosition == -1 ? 0 : this._mSyncPosition;
				this._mChildrenMarginTop = -Math.abs(this._mSyncTop);
				this._mUI.listContainer.style.marginTop = this._mChildrenMarginTop + "px";
				this._fillFromTop(this._mFirstPosition);
				this._fixTooHigh();
				this._computeSyncState();
				this._mSyncId = null;
				break;
			default:
				this._mFirstPosition = 0;
				this._fillFromTop(0);
				this._mSyncTop = 0;
				this._mSyncPosition = -1;
				this._mSyncId = null;
				break;
			}
			this._mViewRecycler.scrapActiveViews();
			this._mLayoutMode = _.LAYOUT_MODE_NORMAL;
			this._mSpecificPosition = -1;
			this._mPrivateFlags &= ~_.DATA_CHANGED;
			this.onPositionChanged(this._mFirstPosition, this._mChildren.length);
			this._turnPluginDriverOnOrOff();
		},
		_turnPluginDriverOnOrOff : function () {
			var C = this.getScrollbar();
			if (C) {
				var E = false,
				D = this._mFirstPosition + this._mChildren.length;
				if (this._mFirstPosition > 0 || D < this._mItemCount) {
					E = true;
				} else {
					if (D == this._mItemCount) {
						var _ = this._mChildrenMarginTop,
						A = (this._mChildren.length * this._mItemHeight) + _,
						B = this._mUI.listContainer.parentNode.offsetHeight;
						if (A > B) {
							E = true;
						}
					}
				}
				C.awake(E);
			}
		},
		_makeAndAddView : function (B) {
			var A = this._mViewRecycler.getActiveView(B);
			if (A == null) {
				var _ = this._mViewRecycler.get(B),
				A = null;
				if (_ != null) {
					A = this.getView(this._mUI.listContainer, _, B);
				} else {
					A = this.getView(this._mUI.listContainer, null, B);
				}
			}
			if (A == null) {
				throw new Error("can not obtain a view to build list item");
			}
			this.insertView(A, B);
			return A;
		},
		insertView : function (_, B) {
			var C = B - this._mFirstPosition,
			A = (0 + this._mChildren.length - 1) >> 1;
			if (C > A) {
				this._mUI.listContainer.appendChild(_);
			} else {
				this._mUI.listContainer.insertBefore(_, this._mChildren[0]);
			}
		},
		removeView : function (_) {
			return this._mUI.listContainer.removeChild(_);
		},
		_fillFromTop : function (G) {
			if (!this._mUI) {
				return;
			}
			var D = this._mUI.listContainer.parentNode.offsetHeight,
			C = this._mChildrenMarginTop,
			A = G - this._mFirstPosition,
			F = this._mItemHeight,
			B = 0;
			if ((this._mPrivateFlags & _.CLONE_VIEW_BUILD) != _.CLONE_VIEW_BUILD) {
				var H = this.getView(this._mUI.listContainer, null, 0);
				this.insertView(H, 0);
				this._mChildren[0] = H;
				this._mItemHeight = H.offsetHeight;
				F = this._mItemHeight;
				B = F;
				G++;
				A++;
				this._mPrivateFlags |= _.CLONE_VIEW_BUILD;
			} else {
				B = A * F + C;
			}
			var E = null;
			while (B < D && G < this._mItemCount) {
				E = this._makeAndAddView(G);
				this._mChildren[A] = E;
				B += F;
				G++;
				A++;
			}
		},
		importCheckedState : function (D) {
			if (D instanceof _) {
				this._mCheckedChildren.length = 0;
				for (var B = 0, C = D._mCheckedChildren, A = C.length; B < A; B++) {
					this._mCheckedChildren[B] = C[B];
				}
				if (this.isAllItemChecked() == true && D.isAllItemChecked() == false) {
					this._mPrivateFlags &= ~_.CHECKED_ALL;
				}
			}
		},
		_fillFromBottom : function (D) {
			if (!this._mUI) {
				return;
			}
			var A = this._mUI.listContainer.parentNode.offsetHeight,
			_ = this._mChildrenMarginTop,
			C = this._mItemHeight,
			E = _,
			B = null;
			while (D >= 0 && E >= 0) {
				B = this._makeAndAddView(D);
				_ -= C;
				this._mUI.listContainer.style.marginTop = _ + "px";
				this._mChildren.unshift(B);
				E -= C;
				D--;
			}
			this._mChildrenMarginTop = _;
			this._mFirstPosition = D + 1;
		},
		fillGap : function (A) {
			var _ = this._mChildren.length;
			if (A == disk.ui.MouseWheelSensor.FORWARD) {
				this._fillFromTop(this._mFirstPosition + _);
				this._fixTooHigh();
			} else {
				this._fillFromBottom(this._mFirstPosition - 1);
				this._fixTooLow();
			}
		},
		_fixTooLow : function () {
			var F = this._mChildren.length;
			if (F <= 0) {
				return;
			}
			var A = this._mChildrenMarginTop,
			C = (this._mChildren.length * this._mItemHeight) + A,
			D = this._mUI.listContainer.parentNode.offsetHeight,
			E = A,
			G = E,
			B = this._mFirstPosition;
			if (B == 0) {
				if (G > 0) {
					if (B + F == this._mItemCount || C > D) {
						if (B + F == this._mItemCount) {
							G = Math.min(G, C - D);
						}
						A -= G;
						this._mUI.listContainer.style.marginTop = A + "px";
						this._mChildrenMarginTop = A;
						if (B + F < this._mItemCount) {
							this._fillFromTop(B + F);
						}
						this.onScrollToEdge(_.TOP);
					}
				}
			}
		},
		_fixTooHigh : function () {
			var F = this._mChildren.length;
			if (F <= 0) {
				return;
			}
			var A = this._mChildrenMarginTop,
			C = (this._mChildren.length * this._mItemHeight) + A,
			D = this._mUI.listContainer.parentNode.offsetHeight,
			E = A,
			G = D - C,
			B = this._mFirstPosition;
			if ((B > 0 || E < 0) && G > 0) {
				if (B == 0) {
					G = Math.min(G, Math.abs(E));
				}
				A += G;
				this._mChildrenMarginTop = A;
				this._mUI.listContainer.style.marginTop = A + "px";
				if (B > 0) {
					this._fillFromBottom(B - 1);
					this._adjustViewUpOrDown();
				}
				this.onScrollToEdge(_.BOTTOM);
			} else {
				if (this._mFirstPosition + F == this._mItemCount && C == D) {
					this.onScrollToEdge(_.BOTTOM);
				}
			}
		},
		_adjustViewUpOrDown : function () {
			var _ = this._mChildren.length;
			if (_ > 0) {
				if (this._mChildrenMarginTop > 0) {
					this._mChildrenMarginTop = 0;
					this._mUI.listContainer.style.marginTop = this._mChildrenMarginTop + "px";
				}
			}
		},
		buildView : function (A, B, _) {
			return null;
		},
		onScrollToEdge : function (_) {},
		onPositionChanged : function (B, A) {
			if ((this._mPrivateFlags & _.USING_SCROLLBAR) == _.USING_SCROLLBAR) {
				this.awakeScrollbar(true);
				this.onComputeScrollbarState(this.getScrollbar(true), B, A);
			}
		},
		onKeyboardArrowDown : function () {
			this.arrowScroll(disk.ui.MouseWheelSensor.FORWARD);
		},
		onKeyboardArrowUp : function () {
			this.arrowScroll(disk.ui.MouseWheelSensor.BACKWARD);
		},
		onKeyboardPageUp : function () {
			this.pageScroll(disk.ui.MouseWheelSensor.BACKWARD);
		},
		onKeyboardPageDown : function () {
			this.pageScroll(disk.ui.MouseWheelSensor.FORWARD);
		},
		getCount : function () {
			return this._mItemCount;
		},
		onScroll : function (J, G) {
			var D = this._mFirstPosition,
			H = this._mChildren.length,
			A = this._mChildrenMarginTop,
			F = A,
			C = (this._mChildren.length * this._mItemHeight) + A,
			E = this._mUI.listContainer.parentNode.offsetHeight;
			G = Math.min(G, E - 1);
			if (J == disk.ui.MouseWheelSensor.FORWARD && D + H == this._mItemCount && C <= E && G >= 0) {
				return G != 0;
			}
			if (J == disk.ui.MouseWheelSensor.BACKWARD && D == 0 && F >= 0 && G >= 0) {
				return G != 0;
			}
			var I = 0;
			if (J == disk.ui.MouseWheelSensor.FORWARD) {
				for (var B = 0, _ = this._mChildren.length; B < _; B++) {
					if ((B + 1) * this._mItemHeight - Math.abs(A) < 0) {
						A += this._mItemHeight;
						this._mUI.listContainer.style.marginTop = A + "px";
						this._mViewRecycler.add(this._mChildren.shift());
						I++;
					} else {
						break;
					}
				}
			} else {
				for (B = this._mChildren.length - 1; B >= 0; B--) {
					if (this._mItemHeight * B + A > E) {
						this._mViewRecycler.add(this._mChildren.pop());
						I++;
					} else {
						break;
					}
				}
			}
			if (J == disk.ui.MouseWheelSensor.FORWARD) {
				A -= G;
			} else {
				A += G;
			}
			this._mUI.listContainer.style.marginTop = A + "px";
			this._mChildrenMarginTop = A;
			if (J == disk.ui.MouseWheelSensor.FORWARD) {
				this._mFirstPosition += I;
			}
			C = (this._mChildren.length * this._mItemHeight) + A;
			if (A > 0 || C < E) {
				this.fillGap(J);
			}
			this._computeSyncState();
			this.onPositionChanged(this._mFirstPosition, this._mChildren.length);
		},
		_computeSyncState : function () {
			this._mSyncTop = this._mChildrenMarginTop;
			this._mSyncPosition = this._mFirstPosition;
			var _ = this.getIdForPosition(this._mFirstPosition);
			if (_ != null) {
				this._mSyncId = _;
			}
		},
		_computeSmoothScrollArgs : function (C) {
			var D = _.SMOOTH_SCROLL_DURATION;
			this._mSmoothScrollDelta = Math.ceil(C / (D / _.SMOOTH_SCROLL_INTERVAL));
			if (this._mSmoothScrollDelta >= this._mItemHeight) {
				var B = 10,
				A = 0;
				while (this._mSmoothScrollDelta >= this._mItemHeight) {
					if (A >= B) {
						throw new Error("pixelDelta arg is not considered as legal");
						break;
					}
					D *= 2;
					this._mSmoothScrollDelta = Math.ceil(C / (D / _.SMOOTH_SCROLL_INTERVAL));
					A++;
				}
			}
		},
		_clearSmoothScrollArgs : function () {
			this._mPrivateFlags &= ~_.SMOOTH_SCROLLING;
			this._mSmoothScrollRemaining = this._mSmoothScrollDelta = 0;
			this._mScrollDir = -1;
		},
		stopSmoothScroll : function () {
			if (this._mSmoothScroller != null) {
				this._mSmoothScroller.interrupt();
				this._clearSmoothScrollArgs();
			}
		},
		_setupSmoothScrollArgs : function (A) {
			var C = this;
			if (C._mSmoothScroller == null) {
				C._mSmoothScroller = new disk.util.TimerService(_.SMOOTH_SCROLL_INTERVAL, function () {
						var _ = false;
						if (Math.abs(C._mSmoothScrollRemaining) <= Math.abs(C._mSmoothScrollDelta)) {
							_ = C.onScroll(C._mScrollDir, C._mSmoothScrollRemaining);
							C._clearSmoothScrollArgs();
							if (!_ && C._mPersistScrollDir != -1) {
								C.arrowScroll(C._mPersistScrollDir);
							} else {
								C._mPersistScrollDir == -1;
							}
						} else {
							C._mSmoothScrollRemaining -= C._mSmoothScrollDelta;
							_ = C.onScroll(C._mScrollDir, C._mSmoothScrollDelta);
							this.start();
						}
					});
			}
			C._computeSmoothScrollArgs(A);
			C._mSmoothScrollRemaining = A - C._mSmoothScrollDelta;
			C._mSmoothScroller.start();
			var B = C.onScroll(C._mScrollDir, C._mSmoothScrollDelta);
			C._mPrivateFlags |= _.SMOOTH_SCROLLING;
		},
		smoothScroll : function (A, _) {
			if (this.locked()) {
				return;
			}
			this._mScrollDir = A;
			this._setupSmoothScrollArgs(_);
		},
		scrollBy : function (A, _) {
			if (this.locked()) {
				return;
			}
			this.onScroll(A, _);
		},
		setUsingKeyboardDispatcher : function (A) {
			if (A) {
				this._mPrivateFlags |= _.USING_KEYBOARD_DISPATCHER;
			} else {
				this._mPrivateFlags &= ~_.USING_KEYBOARD_DISPATCHER;
			}
		},
		activateKeyguard : function (C, A) {
			var B = this._mConfig.flags | this._mPrivateFlags;
			if ((B & _.USING_KEYBOARD_DISPATCHER) == _.USING_KEYBOARD_DISPATCHER) {
				if (C) {
					if (!this._mKeyguard) {
						var D = this;
						this._mKeyguardListener = {
							onMount : function (_) {
								if (disk.DEBUG) {
									console.log("###we are mounted = ", _);
								}
							},
							onConnectivity : function (_) {
								if (disk.DEBUG) {
									console.log("###connectivity=", _);
								}
							},
							onKeyAction : function (_) {
								if (disk.DEBUG) {
									console.log("###key action=", _);
								}
								if (D._mItemCount == 0) {
									return false;
								}
								switch (_) {
								case disk.ui.Keyguard.EVENT_PAGE_UP:
									D.onKeyboardPageUp();
									return true;
								case disk.ui.Keyguard.EVENT_PAGE_DOWN:
									D.onKeyboardPageDown();
									return true;
								case disk.ui.Keyguard.EVENT_ARROW_UP:
									D.onKeyboardArrowUp();
									return true;
								case disk.ui.Keyguard.EVENT_ARROW_DOWN:
									D.onKeyboardArrowDown();
									return true;
								default:
									return false;
								}
							},
							onKeyEvent : function (_, A) {
								if (disk.DEBUG) {
									console.log("###key event=", _, A);
								}
							}
						};
						this._mKeyguard = disk.Context.getService(disk.Context.SERVICE_KEYGUARD);
					}
					this._mKeyguard.acquire(this._mKeyguardListener, A);
				} else {
					this._mKeyguard.release(this._mKeyguardListener);
				}
			}
		},
		isUsingKeyboardDispatcher : function () {
			return (this._mPrivateFlags & _.USING_KEYBOARD_DISPATCHER) == _.USING_KEYBOARD_DISPATCHER;
		},
		setUsingMouseWheelSensor : function (A) {
			if (A) {
				this._mPrivateFlags |= _.USING_MOUSE_WHEEL_SENSOR;
				var C = this,
				B = new disk.ui.MouseWheelSensor(this._mUI.listContainer.parentNode);
				B.onWheelChanged = function (F, D) {
					if (C.locked()) {
						return;
					}
					var E = _.WHEEL_TO_PIXEL_RATIO;
					if ((C._mPrivateFlags & _.USING_LOW_PIXEL_RATIO) == _.USING_LOW_PIXEL_RATIO) {
						E = _.WHEEL_TO_PIXEL_RATIO_LOW;
					}
					var A = D * E;
					if ((C._mPrivateFlags & _.SMOOTH_SCROLLING) == _.SMOOTH_SCROLLING) {
						C.stopSmoothScroll();
					}
					C._mScrollDir = F;
					if (C._mItemHeight == 0) {
						return;
					}
					var B = C.onScroll(C._mScrollDir, A);
				};
				B.sense();
				this._mWheelSensor = B;
			} else {
				this._mPrivateFlags &= ~_.USING_MOUSE_WHEEL_SENSOR;
			}
		},
		_build : function () {
			var C = this;
			this._mViewRecycler = new A();
			var B = this._mConfig.flags;
			if (typeof B != "undefined") {
				if ((B & _.USING_MOUSE_WHEEL_SENSOR) == _.USING_MOUSE_WHEEL_SENSOR) {
					this.setUsingMouseWheelSensor(true);
				}
				if ((B & _.USING_SCROLLBAR) == _.USING_SCROLLBAR) {
					this.setUsingScrollbar(true);
				}
				if ((B & _.USING_KEYBOARD_DISPATCHER) == _.USING_KEYBOARD_DISPATCHER) {
					this.setUsingKeyboardDispatcher(true);
				}
				if ((B & _.USING_TOUCH_SENSOR) == _.USING_TOUCH_SENSOR) {
					if (disk.ui.MotionSensor.hasMotionCampatibility()) {
						if (disk.DEBUG) {
							console.log("Motion Tracker Installed");
						}
						this.setUsingMotionSensor(true);
					}
				}
			}
		},
		setUsingMotionSensor : function (A) {
			if (A) {
				this._mPrivateFlags |= _.USING_TOUCH_SENSOR;
				var C = new disk.ui.MotionSensor(this._mUI.listContainer, disk.ui.MotionSensor.VERTICAL),
				B = this;
				C.onMotion = function (G, E, A, C, F) {
					if (B.locked()) {
						return;
					}
					if ((B._mPrivateFlags & _.SMOOTH_SCROLLING) == _.SMOOTH_SCROLLING) {
						B.stopSmoothScroll();
					}
					B._mScrollDir = C > 0 ? disk.ui.MouseWheelSensor.BACKWARD : disk.ui.MouseWheelSensor.FORWARD;
					if (B._mItemHeight == 0) {
						return;
					}
					var D = B.onScroll(B._mScrollDir, Math.abs(C));
				};
				C.install();
				this._mMotionSensor = C;
			} else {
				this._mPrivateFlags &= ~_.USING_TOUCH_SENSOR;
			}
		},
		setDebugAlias : function (_) {
			this._mDebugAlias = _;
		},
		toString : function () {
			return this._mDebugAlias;
		}
	};
	disk.ui.RecycleListView = _;
})();
(function () {
	var _ = function () {
		this._mPackName = null;
		this._mPublicMode = false;
		this._mDM = null;
		this._mIsFile = false;
		this._mOPackName = null;
		this._mOList = null;
		this._mWatcher = null;
	};
	_.dom = null;
	_._warmupHTML = function () {
		var B = _.dom;
		if (B == null) {
			B = document.createElement("div");
			B.className = "pcs-hide-ele";
			document.body.appendChild(B);
			var A = [];
			A.push('<iframe src="javascript:;" id="pcsdownloadiframe" name="pcsdownloadiframe"></iframe>');
			A.push('<form target="pcsdownloadiframe" enctype="application/x-www-form-urlencoded" action="' + disk.api.RestAPI.MULTI_DOWNLOAD + '" method="post" name="pcsdownloadform">');
			A.push('<input name="method" value="batchdownload" type="hidden" /><input name="zipcontent" type="hidden" /><input name="zipname" type="hidden" /></form>');
			B.innerHTML = A.join("");
			_.dom = B;
		}
	};
	_.isPlatformCompatible = function () {
		var _ = navigator.platform;
		return _.toLowerCase().indexOf("win32") == 0;
	};
	_.removeQuotes = function (_) {
		return _.replace(/['"]/gi, "");
	};
	_.BETA = false;
	_.prototype = {
		_requestDownloadManager : function (_, A) {
			$.getScript("/res/static/js/ui/download_manager.js").done(_).fail(A);
		},
		_calculateMode : function (_) {
			var C = 0,
			B = 0;
			if (typeof _.length == "number") {
				for (var A = 0; A < _.length; A++) {
					C += parseInt(_[A].size);
					parseInt(_[A].isdir) == 1 ? B = 1 : "";
				}
			} else {
				C = parseInt(_.size);
				B = _.isdir;
			}
			if (disk.DEBUG) {
				console.log(C);
			}
			if (typeof _.length != "number") {
				disk.util.DownloadManager.logMsg.category = disk.util.DownloadManager.logMsg.singleFileCategory;
				if (B == 1) {
					return disk.util.DownloadManager.MODE_PRE_INSTALL;
				} else {
					if (C < disk.util.DownloadManager.SMALL_SIZE_FILE) {
						return disk.util.DownloadManager.MODE_DIRECT_DOWNLOAD;
					} else {
						if (C > disk.util.DownloadManager.SMALL_SIZE_FILE && C < disk.util.DownloadManager.SIZE_THRESHOLD) {
							return disk.util.DownloadManager.MODE_PRE_DOWNLOAD;
						} else {
							return disk.util.DownloadManager.MODE_PRE_INSTALL;
						}
					}
				}
			} else {
				disk.util.DownloadManager.logMsg.category = disk.util.DownloadManager.logMsg.multipleFileCategory;
				if (_.length > 100 || C > disk.util.DownloadManager.MEDIUM_SIZE_FILE || B == 1) {
					return disk.util.DownloadManager.MODE_PRE_INSTALL;
				} else {
					return disk.util.DownloadManager.MODE_PRE_DOWNLOAD;
				}
			}
		},
		_test0 : function (_, C) {
			var E = this,
			A = disk.Context.getService(disk.Context.SERVICE_TOAST);
			A.setMessage(disk.ui.Toast.MODE_LOADING, "\u6b63\u5728\u52a0\u8f7d\u7ec4\u4ef6\uff0c\u8bf7\u7a0d\u5019&hellip;");
			A.setVisible(true, true);
			var B = function () {
				disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
				var A = E._mDM;
				if (A == null) {
					A = new disk.util.DownloadManager(_, C, E._mIsFile, E._mOPackName, E._mOList);
					E._mDM = A;
				} else {
					A.setDependencyData(_, C, E._mIsFile, E._mOPackName, E._mOList);
				}
				if (typeof A.addVisibilityEventWatcher == "function") {
					if (typeof E._mWatcher == "function") {
						A.addVisibilityEventWatcher(E._mWatcher);
					} else {
						A.addVisibilityEventWatcher(null);
					}
				}
				A.setMode(E._calculateMode(_));
				A.setVisible(true);
			},
			D = function () {
				var _ = disk.Context.getService(disk.Context.SERVICE_TOAST);
				_.setMessage(disk.ui.Toast.MODE_CAUTION, "\u7ec4\u4ef6\u52a0\u8f7d\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5");
				_.setVisible(true, false);
			};
			if (this._mDM || typeof disk.util.DownloadManager == "function") {
				B.call(B);
			} else {
				this._requestDownloadManager(B, D);
			}
		},
		assignPureList : function (_) {
			this._mOList = _;
		},
		straightforwardDownload : function (_, B) {
			if ((B === 1 && _.dlink) || (B !== 1 && B !== true)) {
				this._doDownloadFile(_, B);
			} else {
				this._checkDownloadFile(_, B, "", "", true);
			}
			try {
				var A = "";
				if (B === 1) {
					A = "downloadfile|downloadSize_" + _.size + "|downloadFileLength_1|downloadFileCategory_." + _.path.split(".")[_.path.split(".").length - 1];
				} else {
					A = "downloadfile|downloadFileLength_" + _.length;
				}
				FileUtils._mDiskLog.send({
					type : A,
					md5 : (_.md5 || "proxypcs")
				});
			} catch (C) {}

		},
		_doDownloadFile : function (A, C) {
			_._warmupHTML();
			if (C) {
				var B = document.getElementById("pcsdownloadiframe");
				B.src = Page.obtainDownloadURL(A);
			} else {
				this._mPublicMode = C === true;
				this._assignPostFormTask(A);
			}
		},
		_useAdvanceFeature : function () {
			if (!_.BETA) {
				return true;
			}
			return disk.ui.Utilities.getBetaFeature("expvar");
		},
		download : function (B, F) {
			if (!_.isPlatformCompatible() || !this._useAdvanceFeature()) {
				var D;
				if (F == true && B instanceof Array) {
					D = [];
					var E;
					for (var C = 0, A = B.length; C < A; C++) {
						E = B[C];
						if (E && E.fs_id) {
							D.push(E.fs_id);
						}
					}
				} else {
					D = B;
				}
				this.straightforwardDownload(D, F);
				return;
			}
			this._test0(B, F);
		},
		_resolveExtraInfo : function () {
			var A = [],
			_ = FileUtils.share_id || "";
			if (_.length > 0) {
				A.push("&shareid=");
				A.push(_);
			}
			_ = disk.util.HistoryManager.parseHistory(location.hash.substring(1));
			if (_ != null) {
				A.push("&path=");
				A.push(encodeURIComponent(_[1].path));
			}
			return A.join("");
		},
		addUIVisibilityEventWatcher : function (_) {
			this._mWatcher = _;
		},
		_assignPostFormTask : function (_) {
			var A = document.forms.pcsdownloadform,
			B = null;
			if (this._mPublicMode) {
				A.action = disk.api.RestAPI.MULTI_DOWNLOAD_PUBLIC + "&uk=" + FileUtils.sysUK + this._resolveExtraInfo();
				B = _;
			} else {
				A.action = disk.api.RestAPI.MULTI_DOWNLOAD;
				B = {
					list : _
				};
			}
			A.elements.zipcontent.value = $.stringify(B);
			A.elements.zipname.value = this._mPackName;
			A.submit();
		},
		setPackName : function (A, B) {
			this._mIsFile = B;
			this._mOPackName = A;
			if (B === true) {
				var _ = A.lastIndexOf(".");
				if (_ != -1) {
					A = A.substring(0, _);
				}
			}
			this._mPackName = "\u3010\u6279\u91cf\u4e0b\u8f7d\u3011" + A + "\u7b49.zip";
		}
	};
	_.prototype._showDownloadVCodeDialog = function (_, J, I, H, B, F) {
		var E = this,
		C = ['<div class="download-verify" id="downloadVerify">', '<div class="verify-body">\u8bf7\u8f93\u5165\u9a8c\u8bc1\u7801\uff1a<input type="text" class="input-code" maxlength="4">', '<img class="img-code" alt="\u9a8c\u8bc1\u7801\u83b7\u53d6\u4e2d" src="', I, '" width="100" height="30" />', '<a href="javascript:;" class="underline">\u6362\u4e00\u5f20</a>', "</div>", '<div class="verify-error">', (B ? "" : "\u9a8c\u8bc1\u7801\u8f93\u5165\u9519\u8bef\uff0c\u8bf7\u91cd\u65b0\u8f93\u5165"), "</div>", "</div>"],
		A = disk.Context.getService(disk.Context.SERVICE_DIALOG),
		G = $(".alert-dialog .okay"),
		D = function () {
			var C = $("#downloadVerify .input-code"),
			B = $("#downloadVerify .verify-error"),
			A = $.trim(C.val());
			if (A == "") {
				B.text("\u8bf7\u8f93\u5165\u9a8c\u8bc1\u7801");
				C.focus();
				return;
			}
			if (A.length != 4) {
				B.text("\u9a8c\u8bc1\u7801\u8f93\u5165\u9519\u8bef\uff0c\u8bf7\u91cd\u65b0\u8f93\u5165");
				C.focus();
				return;
			}
			B.text("");
			E._checkDownloadFile(_, J, A, H, false, F);
		};
		G.html("<b>\u786e\u5b9a</b>").removeClass("disabled");
		A.setMessage("\u63d0\u793a", C.join(""), function () {
			D();
		}, function () {
			if (typeof E._mWatcher == "function") {
				E._mWatcher.call(E, false);
			}
		});
		A.setVisible(true);
		if (typeof E._mWatcher == "function") {
			E._mWatcher.call(E, true);
		}
		var K = $("#downloadVerify .input-code");
		$("#downloadVerify .underline").click(function () {
			$("#downloadVerify .img-code").attr("src", I + "&" + (+new Date()));
			K.focus();
		});
		K.blur(function () {
			if (K.val()) {
				$("#downloadVerify .verify-error").text("");
			}
		}).keydown(function (_) {
			if (_.keyCode == 13) {
				D();
				return false;
			}
		}).focus();
	};
	_.prototype._startDownloadFile = function (B, A) {
		var C = this,
		_ = {
			dlink : A + (B === true ? ("&zipname=" + encodeURIComponent(C._mPackName)) : "")
		};
		C._doDownloadFile(_, B);
	};
	_.prototype._switchDownloadTips = function (C, _) {
		if (C === false) {
			return;
		}
		var D = this,
		A = $.browser,
		B = false;
		if (A.msie && parseInt(A.version, 10) <= 8) {
			B = true;
		}
		if (B) {
			D._showDownloadingDialog(C, _);
		} else {
			if (_) {
				D._startDownloadFile(C, _);
			}
		}
	};
	_.prototype._showDownloadingDialog = function (E, B) {
		var F = this,
		C = ['<div class="download-verify-tips">', B ? "\u4e0b\u8f7d\u94fe\u63a5\u5df2\u751f\u6210\uff0c\u8bf7\u70b9\u51fb\u4e0b\u8f7d\u3002" : "<span>\u6b63\u5728\u751f\u6210\u4e0b\u8f7d\u94fe\u63a5\uff0c\u8bf7\u7a0d\u5019...</span>", "</div>"],
		A = disk.Context.getService(disk.Context.SERVICE_DIALOG),
		_ = $(".alert-dialog .okay"),
		D;
		if (B) {
			D = function () {
				disk.Context.getService(disk.Context.SERVICE_DIALOG).setVisible(false);
				if (typeof F._mWatcher == "function") {
					F._mWatcher.call(F, false);
				}
				F._startDownloadFile(E, B);
				try {
					FileUtils._mDiskLog.send({
						type : "download_browser_lteq_ie8"
					});
				} catch (_) {}

			};
		} else {
			D = function () {};
		}
		if (B) {
			_.html("<b>\u7acb\u5373\u4e0b\u8f7d</b>").removeClass("disabled");
		} else {
			_.html("<b>\u751f\u6210\u4e0b\u8f7d\u94fe\u63a5...</b>").addClass("disabled");
		}
		A.setMessage("\u63d0\u793a", C.join(""), function () {
			D();
		}, function () {
			_.html("<b>\u786e\u5b9a</b>").removeClass("disabled");
			if (typeof F._mWatcher == "function") {
				F._mWatcher.call(F, false);
			}
		});
		A.setVisible(true);
		if (typeof F._mWatcher == "function") {
			F._mWatcher.call(F, true);
		}
	};
	_.prototype._checkDownloadFile = function (_, K, A, H, B, F) {
		var C;
		if (K === 1) {
			C = [_.fs_id];
		} else {
			C = _;
		}
		var D = this,
		L = disk.api.RestAPI.SHARE_GET_DLINK + "&uk=" + FileUtils.share_uk + "&shareid=" + FileUtils.share_id + "&timestamp=" + FileUtils.share_timestamp + "&sign=" + FileUtils.share_sign,
		I = {
			fid_list : $.stringify(C)
		};
		if (A && H) {
			I.input = A;
			I.vcode = H;
		}
		if (K === false) {
			L += "&nozip=1";
		}
		if (typeof F != "function") {
			D._switchDownloadTips(K, "");
		}
		var J = function (_) {
			disk.Context.getService(disk.Context.SERVICE_DIALOG).setVisible(false);
			if (typeof D._mWatcher == "function") {
				D._mWatcher.call(D, false);
			}
			if (typeof F == "function") {
				F(_);
			} else {
				D._switchDownloadTips(K, _);
			}
		},
		G = function (C, A) {
			D._showDownloadVCodeDialog(_, K, C, A, B, F);
		},
		E = function (_) {
			var A = disk.Context.getService(disk.Context.SERVICE_TOAST),
			B = "";
			if (_) {
				B = disk.util.shareErrorMessage[_];
			}
			B = B || "网络错误，请稍候重试";
			A.setMessage(disk.ui.Toast.MODE_CAUTION, B);
			A.setVisible(true, false);
			disk.Context.getService(disk.Context.SERVICE_DIALOG).setVisible(false);
		};
		$.post(L, I, function (A) {
			var _ = null;
			try {
				_ = $.parseJSON(A);
			} catch (B) {}

			if (_) {
				if (_.errno == 0) {
					if (_.dlink) {
						J(_.dlink);
					} else {
						if (_.list) {
							J(_.list);
						} else {
							E();
						}
					}
				} else {
					if (_.errno == -19 && _.img && _.vcode) {
						G(_.img, _.vcode);
					} else {
						if (_ && _.errno) {
							E(_.errno);
						} else {
							E();
						}
					}
				}
			} else {
				E();
			}
		}).error(function () {
			E();
		});
	};
	disk.util.DownloadProxy = _;
})();
(function () {
	var _ = function () {
		this._mContentId = disk.obtainId();
		this._mCloseId1 = disk.obtainId();
		this._mCloseId2 = disk.obtainId();
		this._mMoveId = disk.obtainId();
		this._mNewDirId = disk.obtainId();
		this._mTitleId = disk.obtainId();
		this._mActionType = null;
		this.onConsent = null;
		this.onRefuse = null;
		this.onClose = null;
		this._startPath = null;
		this._mCacheItems = null;
		disk.ui.Panel.call(this, this._prebuild(this._mContentId, this._mCloseId1, this._mCloseId2, this._mMoveId, this._mNewDirId, this._mTitleId));
		this._init();
	};
	_.active = null;
	_.COPY = 1;
	_.MOVE = 2;
	_.SELECT_PATH = 3;
	_.prototype = new disk.ui.Panel();
	_.prototype._prebuild = function (D, C, E, B, F, A) {
		var _ = document.createElement("div");
		_.className = "b-panel b-dialog move-dialog";
		_.innerHTML = '<div class="dlg-hd b-rlv"><span title="\u5173\u95ed" id="' + C + '" class="dlg-cnr dlg-cnr-r"></span> <h3><strong id="' + A + '" class="f333">\u79fb\u52a8\u5230</strong></h3></div><div class="dlg-bd"><div id="' + D + '" class="dlg-inner-bd b-bdr-1"></div></div><div class="dlg-ft b-rlv"><div class="alert-dialog-commands clearfix"><a id="' + F + '" href="javascript:void(0)" class="ibtn b-fl"><em class="icon-creat-file"></em><b>\u65b0\u5efa\u6587\u4ef6\u5939</b></a><a id="' + E + '" href="javascript:;" class="abtn b-fr"><b>\u53d6\u6d88</b></a><a id="' + B + '" href="javascript:;" class="sbtn b-fr"><b>\u786e\u5b9a</b></a></div></div>';
		document.body.appendChild(_);
		return {
			pane : _
		};
	};
	_.prototype.setCacheItems = function (_) {
		this._mCacheItems = _;
	};
	_.prototype.getCacheItems = function () {
		return this._mCacheItems;
	};
	_.prototype.setActionType = function (B) {
		this._mActionType = B;
		var A;
		if (B == _.MOVE) {
			A = "\u79fb\u52a8\u5230";
		} else {
			if (B == _.COPY) {
				A = "\u590d\u5236\u5230";
			} else {
				A = "\u4fdd\u5b58\u5230";
			}
		}
		$("#" + this._mTitleId).text(A);
	};
	_.prototype.setDialogTitle = function (_) {
		var A = _ || "\u4fdd\u5b58\u5230";
		$("#" + this._mTitleId).text(A);
	};
	_.prototype.sendRefuseMessage = function () {
		if (typeof this.onRefuse == "function") {
			this.onRefuse.call(this, this._mActionType);
		}
	};
	_.prototype.setVisible = function (A) {
		if (!A) {
			var _ = disk.ui.FolderBuilder.getDefault();
			if (_.isVisible()) {
				_.dispatchNegativeAction();
			}
		}
		disk.ui.Panel.prototype.setVisible.call(this, A);
	};
	_.prototype._init = function () {
		var _ = this;
		$("#" + this._mCloseId2 + ", #" + this._mCloseId1).click(function (A) {
			_.setVisible(false);
			if (typeof _.onClose == "function") {
				_.onClose.call(_);
			}
		});
		$(window).bind("resize", function () {
			_.setGravity(disk.ui.Panel.CENTER);
		});
		$("#" + this._mMoveId).click(function (C) {
			if (disk.DEBUG) {
				console.log("move dialog>>>", _);
			}
			_.setVisible(false);
			if (typeof _.onConsent == "function") {
				var A = disk.ui.TreeLeaf.activeLeaf,
				B = (A == null ? "" : A.getHost().getAttr("dir") == FileUtils.ROOT_ID ? "" : A.getHost().getAttr("dir"));
				if (FileUtils && typeof FileUtils.operateCachePath == "function") {
					FileUtils.operateCachePath(true, B);
				}
				_.onConsent.call(_, _._mActionType, B);
			}
		});
	};
	_.prototype.onVisibilityChange = function (A) {
		disk.Context.getService(disk.Context.SERVICE_CANVAS).setVisible(A);
		if (A) {
			disk.ui.MoveCopyDialog.active = this;
			this.setGravity(disk.ui.Panel.CENTER);
			var B = this,
			_ = "";
			if (B._startPath != null) {
				_ = B._startPath;
			} else {
				if (FileUtils.inHomeMode() && (!FileUtils.inSearchMode())) {
					_ = FileUtils.getDirMgr().getDir();
				}
			}
			if (disk.DEBUG) {
				console.warn("select Path\uff1a", _);
			}
			disk.Context.requestFeature(disk.Context.FEATURE_TREEVIEW, {
				host : document.getElementById(this._mContentId),
				newDirectoryTrigger : $("#" + this._mNewDirId),
				path : _,
				rebuild : FileUtils.getLocalCache().isDirty(),
				callback : function () {
					if (disk.DEBUG) {
						console.log("before request layout");
					}
					B.requestLayoutTreeview();
					disk.ui.Utilities.bringNodeToFrontInTreeview($("#" + B._mContentId));
				}
			});
			FileUtils.getLocalCache().markDirty(false);
		} else {
			this._startPath = null;
		}
	};
	_.prototype.requestLayoutTreeview = function () {
		var _ = $("#" + this._mContentId + " .treeview")[0];
		$(_).width("auto");
		$(_).width(document.getElementById(this._mContentId).scrollWidth);
	};
	disk.ui.MoveCopyDialog = _;
})();
(function () {
	var _ = function () {
		this._iframeId = disk.obtainId();
		this._iframeObj = null;
		this._mContentId = disk.obtainId();
		this._mCloseId1 = disk.obtainId();
		this._mCloseId2 = disk.obtainId();
		this._mMoveId = disk.obtainId();
		this._mNewDirId = disk.obtainId();
		this._mTitleId = disk.obtainId();
		this._innerContentId = disk.obtainId();
		this.onConsent = null;
		this.onRefuse = null;
		this.onClose = null;
		this._mCacheItems = null;
		disk.ui.Panel.call(this, this._prebuild(this._mContentId, this._mCloseId1, this._mCloseId2, this._mMoveId, this._mNewDirId, this._mTitleId, this._iframeId));
		this._init();
	};
	_.active = null;
	_.prototype = new disk.ui.Panel();
	_.prototype._prebuild = function (D, C, E, B, F, A, G) {
		var _ = document.createElement("div");
		_.className = "b-panel b-dialog move-dialog";
		_.innerHTML = '<div class="dlg-hd b-rlv"><span title="\u5173\u95ed" id="' + C + '" class="dlg-cnr dlg-cnr-r"></span> <h3><strong id="' + A + '" class="f333">\u4fdd\u5b58\u5230</strong></h3></div><div class="dlg-bd"><div id="' + D + '" class="dlg-inner-bd b-bdr-1"><iframe id=\'' + G + "' frameBorder='none' style='border:0 none;width:100%;height:196px;'></iframe></div></div><div class=\"dlg-ft b-rlv\"><div class=\"alert-dialog-commands clearfix\"><a id=\"" + F + '" href="javascript:void(0)" class="ibtn b-fl disabled"><em class="icon-creat-file"></em><b>\u65b0\u5efa\u6587\u4ef6\u5939</b></a><a id="' + E + '" href="javascript:;" class="abtn b-fr"><b>\u53d6\u6d88</b></a><a id="' + B + '" href="javascript:;" class="sbtn b-fr disabled"><b>\u786e\u5b9a</b></a></div></div>';
		document.body.appendChild(_);
		document.getElementById(G).src = "/share/selectdir";
		return {
			pane : _
		};
	};
	_.prototype.refreshTree = function () {
		this._iframeObj.contentWindow.location.reload();
	};
	_.prototype.getDialogDocument = function () {
		var _ = this._iframeObj;
		return _ && typeof(_) == "object" && _.tagName == "IFRAME" && _.contentDocument || _.contentWindow && _.contentWindow.document || _.document;
	};
	_.prototype._init = function () {
		var _ = this;
		_._iframeObj = $("#" + this._iframeId)[0];
		$("#" + this._mCloseId2 + ", #" + this._mCloseId1).click(function (A) {
			_.setVisible(false);
			if (typeof _.onClose == "function") {
				_.onClose.call(_);
			}
		});
		$(window).bind("resize", function () {
			_.setGravity(disk.ui.Panel.CENTER);
		});
		$("#" + this._mNewDirId).bind("click", function () {
			if ($(this).hasClass("disabled")) {
				return;
			}
			_.getDialogDocument().getElementById("_newDirectoryBtn").click();
		});
		$("#" + this._mMoveId).click(function (C) {
			if (disk.DEBUG) {
				console.log("move dialog>>>", _);
			}
			if ($(this).hasClass("disabled")) {
				return;
			}
			_.setVisible(false);
			if (typeof _.onConsent == "function") {
				var A = _._iframeObj.contentWindow.disk.ui.TreeLeaf.activeLeaf,
				B = (A == null ? "" : A.getHost().getAttr("dir") == _._iframeObj.contentWindow.FileUtils.ROOT_ID ? "" : A.getHost().getAttr("dir"));
				_.onConsent.call(_, B);
			}
		});
	};
	_.prototype.onVisibilityChange = function (_) {
		disk.Context.getService(disk.Context.SERVICE_CANVAS).setVisible(_);
		if (_) {
			this.setGravity(disk.ui.Panel.CENTER);
		}
	};
	_.prototype.requestLayoutTreeview = function () {};
	disk.ui.MoveSaveDialog = _;
})();
(function () {
	var _ = function () {
		this._mStack = [];
		this._mStackShadow = [];
		this.onDirChange = null;
		this._mAfterSearch = false;
		this._mHidingLevel = 0;
		this._mHidingBasePath = "";
		this._mPathFilter = null;
	};
	_.DIR_PARENT = "..";
	_.DIR_ROOT = "";
	_.SEPARATOR = '<span class="gray">&nbsp;&nbsp;&gt&nbsp;&nbsp;</span>';
	_.toDirArray = function (B) {
		var _ = [B],
		A = -1;
		while (-1 != (A = B.lastIndexOf("/"))) {
			_.unshift(B = B.substring(0, A));
		}
		return _;
	};
	_.inRootDir = function (A) {
		var _ = A.indexOf("/");
		return _ != -1 && _ == A.lastIndexOf("/");
	};
	_.isDecendentsOf = function (D, C) {
		if (D.trim().length == 0 || C.trim().length == 0) {
			return false;
		}
		var B = D.substring(1).split("/"),
		E = C.substring(1).split("/");
		if (E.length > B.length) {
			return false;
		}
		for (var A = 0, _ = E.length; A < _; A++) {
			if (B[A] != E[A]) {
				return false;
			}
		}
		return true;
	};
	_.prototype = {
		cd : function (H, E, G) {
			var B = null;
			if (H == _.DIR_PARENT) {
				this._mStack.pop();
				this._mStackShadow.pop();
				B = this.atRoot() ? _.DIR_ROOT : this._mStackShadow[this._mStackShadow.length - 1];
			} else {
				if (H == _.DIR_ROOT) {
					this._mStack.length = 0;
					this._mStackShadow.length = 0;
					B = _.DIR_ROOT;
				} else {
					if (!this._mAfterSearch) {
						if (!E) {
							this._mStackShadow.push(H);
							this._mStack.push(FileUtils.parseDirFromPath(H));
						} else {
							this._mStackShadow.push(H);
							this._mStack.push(H);
						}
					} else {
						var F = H.substring(1).split("/");
						for (var A in F) {
							if (F.hasOwnProperty(A)) {
								var D = "";
								for (var C = 0; C <= A; ++C) {
									D += "/" + F[C];
								}
								this._mStack.push(F[A]);
								this._mStackShadow.push(D);
							}
						}
						this._mAfterSearch = false;
					}
					B = H;
				}
			}
			if (G !== true) {
				this.sendDirChangedMessage(B);
			}
			return B;
		},
		setPathFilter : function (_) {
			this._mPathFilter = _;
		},
		sendDirChangedMessage : function (_) {
			if (typeof this.onDirChange == "function") {
				this.onDirChange.call(this, _);
			}
		},
		switchDir : function (B) {
			var C = null;
			for (var A = 0, _ = B.length; A < _; A++) {
				C = this.cd(B[A], false, true);
			}
			this.sendDirChangedMessage(C);
		},
		getDir : function (A) {
			if (A == _.DIR_PARENT) {
				if (this._mStackShadow.length <= 1) {
					return _.DIR_ROOT;
				}
				return this._mStackShadow[this._mStackShadow.length - 2];
			} else {
				if (this.atRoot()) {
					return _.DIR_ROOT;
				}
				return this._mStackShadow[this._mStackShadow.length - 1];
			}
		},
		getSimpleDir : function (A) {
			if (A == _.DIR_PARENT) {
				if (this._mStack.length <= 1) {
					return _.DIR_ROOT;
				}
				return this._mStack[this._mStack.length - 2];
			} else {
				if (this.atRoot()) {
					return _.DIR_ROOT;
				}
				return this._mStack[this._mStack.length - 1];
			}
		},
		lookback : function (B) {
			if (B > 0 && B <= this._mStack.length) {
				var A = null;
				while (B-- > 0) {
					A = this.cd(_.DIR_PARENT, false, true);
				}
				A = this._translateRoot(A);
				this.sendDirChangedMessage(A);
			}
		},
		_translateRoot : function (A) {
			if (this._mHidingLevel > 0 && A == this._mHidingBasePath) {
				return _.DIR_ROOT;
			}
			return A;
		},
		getLookback : function (A) {
			if (A > 0 && A <= this._mStack.length) {
				if (A == this._mStack.length) {
					return _.DIR_ROOT;
				}
				var B = this._mStackShadow[this._mStack.length - A - 1];
				B = this._translateRoot(B);
				return B;
			}
			return _.DIR_ROOT;
		},
		setHidingInfo : function (A, _) {
			this._mHidingLevel = A;
			this._mHidingBasePath = _;
		},
		toCrumbString : function (G) {
			var A = [];
			if (this._mHidingLevel > 0) {
				for (var C = this._mHidingLevel; C < this._mStack.length; C++) {
					A.push(this._mStack[C]);
				}
			} else {
				A = this._mStack;
			}
			var B = [],
			J = 3,
			I = 0,
			F = 30;
			if (A.length >= J) {
				F = 15;
			}
			for (C = A.length - 1; C >= 0; C--) {
				if (I > J) {
					B.unshift('<span class="">...</span>');
					break;
				} else {
					var E = A[C];
					if (this._mPathFilter != null) {
						E = this._mPathFilter.call(this, E);
					}
					if (E.length > F && (C !== A.length - 1)) {
						E = E.substring(0, F) + "...";
					}
					B.unshift(C == A.length - 1 ? '<span class="last">' + disk.ui.Utilities.encodeHTML(E) + "</span>" : '<a class="b-no-ln" deep="' + I + '" href="javascript:;">' + disk.ui.Utilities.encodeHTML(E) + "</a>");
				}
				I++;
			}
			B.unshift("");
			var H = A.length > 0 ? "" : " disabled",
			D = A.length > 0 ? "<span>\u8fd4\u56de\u4e0a\u4e00\u7ea7</span>|" : "";
			return '<li><a id="parentDir" deep="1" href="javascript:;" class="a-back' + H + '">' + D + "</a></li>" + '<li id="dirPath"><a deep="' + A.length + '" id="dirRoot" href="javascript:;" class="b-no-ln' + H + '">' + G + "</a>" + B.join(_.SEPARATOR) + '</li></li><li id="dirData"><span><span class="loadedDate">-</span></span></li>';
		},
		atRoot : function () {
			return this._mStack.length == 0;
		},
		toString : function () {
			return this.toFriendlyString();
		}
	};
	disk.util.DirectoryLocation = _;
})();
(function () {
	var _ = function () {
		this._mCache = {};
		this._mDirty = false;
	};
	_.prototype = {
		removeAll : function () {
			this._mCache = {};
			this._mDirty = true;
			return this;
		},
		markDirty : function (_) {
			this._mDirty = _;
		},
		isDirty : function () {
			return this._mDirty;
		},
		add : function (_, A) {
			this._mCache[_] = A;
		},
		appendList : function (A, B) {
			var _ = this._mCache[A] || null;
			if (disk.DEBUG) {
				console.assert(_ != null);
			}
			if (_) {
				this._mCache[A] = _.concat(B);
			}
			this._mDirty = true;
		},
		insert : function (A, B) {
			var _ = this._mCache[A] || null;
			if (disk.DEBUG) {
				console.assert(_ != null);
			}
			if (_) {
				_.unshift(B);
			}
			this._mDirty = true;
		},
		muteRemove : function (_) {
			delete this._mCache[_];
		},
		remove : function (_) {
			delete this._mCache[_];
			this._mDirty = true;
		},
		descendentlyRemove : function (_, A) {
			this._removeDescendent(_, A === true ? true : false);
			this._mDirty = true;
		},
		preseveAndDescendentlyRemove : function (C, B) {
			var A = this._mCache[C] || null;
			if (A != null) {
				for (var D = 0, _ = A.length; D < _; D++) {
					if (B == FileUtils.parseDirPath(A[D].path)) {
						A.splice(D, 1);
						this._removeDescendent(B, false);
						break;
					}
				}
			}
			this._mDirty = true;
		},
		_removeDescendent : function (_, B) {
			for (var A in this._mCache) {
				if (this._mCache.hasOwnProperty(A)) {
					if (B === true && A == _) {
						continue;
					}
					if (A.indexOf(_) == 0) {
						if (this._mCache[A].length) {
							this.removeCategorys();
						}
						delete this._mCache[A];
					}
				}
			}
			this._mDirty = true;
		},
		update : function (A, _, B) {
			this._mCache[A] = _;
			if (B === true) {
				this._removeDescendent(A, true);
			}
			this._mDirty = true;
		},
		contains : function (_) {
			return typeof this._mCache[_] != "undefined";
		},
		obtain : function (_) {
			return this._mCache[_ || "0"] || null;
		},
		obtainByAttr : function (A, F, C) {
			if (typeof timestamp == "undefined") {
				var _ = this._mCache[A || "0"],
				E = [];
				if (_) {
					for (var B = 0, D = _.length; B < D; B++) {
						if (_[B][F] == C) {
							E.push(_[B]);
						}
					}
					return E;
				} else {
					return null;
				}
			}
		}
	};
	disk.util.LocalCache = _;
	Function.prototype.method = function (_, A) {
		this.prototype[_] = A;
		return this;
	};
	var A = function () {
		_.call(this);
		this._typeTable = null;
		this._pathByType = {};
	};
	A.KEY_MODE_SEARCH = 0;
	A.KEY_MODE_CATEGORY = 1;
	A.KEY_MODE_NORMAL = 2;
	A.KEY_PREFIX_SEARCH = '\u641c\u7d22:"';
	A.KEY_SUFFIX_SEARCH = '"';
	A.KEY_PREFIX_CATEGORY = '\u5206\u7c7b:"';
	A.KEY_SUFFIX_CATEGORY = '"';
	A.isKeyQulifier = function (_) {
		return (typeof _ == "string" && _.length > 0) || (typeof _ == "number");
	};
	A.getCategoryCacheKeyByIdentifier = function (_) {
		return A.KEY_PREFIX_CATEGORY + FileUtils.TYPES[_ - 1][1] + A.KEY_SUFFIX_CATEGORY;
	};
	A.prototype = new _();
	A.method("add", function (_, B) {
		var F = this._mCache[_];
		if (F) {
			var E = {};
			for (var D in F) {
				if (F.hasOwnProperty(D)) {
					var H = F[D].path,
					G = H.slice(H.lastIndexOf("/"));
					E[G] = true;
				}
			}
			for (var A = 0, C = B.length; A < C; A++) {
				H = B[A].path,
				G = H.slice(H.lastIndexOf("/"));
				E[G] || F.push(B[A]);
			}
			delete this._pathByType[_];
		} else {
			this._mCache[_] = B;
		}
		this._TimeStamp || (this._TimeStamp = {});
		this._TimeStamp[_] = new Date().getTime();
	}).method("_renameVictim", function (A, F, E, C) {
		var D = F.path;
		for (var _ = A.length, B = _ - 1; B >= 0; B--) {
			if (A[B].server_filename == F.server_filename) {
				A[B].path = E;
				A[B].server_filename = C;
				this._removeDescendent(D, false);
				return true;
			}
		}
		return false;
	}).method("_putAsideVictims", function (B, I, A) {
		var F = null,
		D = false,
		H = null,
		E = false;
		for (var _ = B.length, C = _ - 1; C >= 0; C--) {
			for (var J = I.length, G = J - 1; G >= 0; G--) {
				F = I[G];
				H = F.path || F;
				if (B[C].path == H) {
					I.splice(G, 1);
					if (A === true) {}
					else {
						this._removeDescendent(F, false);
					}
					D = true;
					E = true;
				}
			}
			if (E) {
				B.splice(C, 1);
				E = false;
			}
		}
		return D;
	}).method("obtainByMode", function (B, C) {
		var _ = null;
		switch (B) {
		case A.KEY_MODE_SEARCH:
			_ = this._mCache[A.KEY_PREFIX_SEARCH + C + A.KEY_SUFFIX_SEARCH] || null;
			break;
		case A.KEY_MODE_CATEGORY:
			_ = this._mCache[A.getCategoryCacheKeyByIdentifier(C)] || null;
			break;
		case A.KEY_MODE_NORMAL:
			_ = this._mCache[C] || null;
			break;
		default:
			break;
		}
		return _;
	}).method("setCacheByMode", function (B, C, _) {
		this.removeAll();
		switch (B) {
		case A.KEY_MODE_SEARCH:
			this._mCache[A.KEY_PREFIX_SEARCH + C + A.KEY_SUFFIX_SEARCH] = _;
			break;
		case A.KEY_MODE_CATEGORY:
			this._mCache[A.getCategoryCacheKeyByIdentifier(C)] = _;
			break;
		case A.KEY_MODE_NORMAL:
			this._mCache[C] = _;
			break;
		default:
			break;
		}
	}).method("deleteByMode", function (C, D, E) {
		if (!A.isKeyQulifier(D)) {
			throw new Error("identifier should " + "be a legal key(for search, category, or directory");
		}
		if (!(E instanceof Array)) {
			E = [E];
		}
		var B = this.obtainByMode(C, D),
		_ = C == A.KEY_MODE_CATEGORY;
		if (B) {
			if (this._putAsideVictims(B, E, _)) {
				if (_ == true) {
					this.removeAll();
					this._mCache[A.getCategoryCacheKeyByIdentifier(D)] = B;
				} else {
					this.removeCategorys();
					this._mDirty = true;
				}
				return true;
			}
		}
		return false;
	}).method("renameByMode", function (D, F, E, C, B) {
		if (!A.isKeyQulifier(F)) {
			throw new Error("identifier should " + "be a legal key(for search, category, or directory");
		}
		var _ = this.obtainByMode(D, F);
		if (_) {
			if (this._renameVictim(_, E, C, B) && D == A.KEY_MODE_NORMAL) {
				this.removeCategorys();
				this._mDirty = true;
				return true;
			}
		}
		return false;
	}).method("movecopyByMode", function (C, D, E, F) {
		if (!A.isKeyQulifier(D)) {
			throw new Error("identifier should " + "be a legal key(for search, category, or directory");
		}
		var B = this.obtainByMode(C, D),
		_ = C == A.KEY_MODE_CATETORY;
		if (B) {
			if (E == disk.ui.MoveCopyDialog.COPY || (E == disk.ui.MoveCopyDialog.MOVE && this._putAsideVictims(B, F, _))) {
				this.setCacheByMode(C, D, B);
				return true;
			}
		}
		return false;
	}).method("insert", function (A, B) {
		var _ = this._mCache[A] || null;
		if (disk.DEBUG) {
			console.assert(_ != null);
		}
		if (_) {
			_.unshift(B);
			if (!B.isdir) {
				var D = this._typeTable,
				C = this._mCache['\u5206\u7c7b:"' + FileUtils.TYPES[FileUtils.TYPESNAMEHASH[B.category] - 1][1] + '"'];
				C && C.unshift(B);
				if (D) {
					var E = D[B.category];
					E.count++;
					E.size += B.size;
				}
			}
		}
		this._mDirty = true;
	}).method("preseveAndDescendentlyRemove", function (_, F) {
		var A = this._mCache[_] || null;
		if (A != null) {
			for (var G = 0, B = A.length; G < B; G++) {
				if (F == FileUtils.parseDirPath(A[G].path)) {
					var L = A[G],
					I = this._typeTable;
					if (I && !L.isdir) {
						var C = this._mCache['\u5206\u7c7b:"' + FileUtils.TYPES[FileUtils.TYPESNAMEHASH[A[G].category] - 1][1] + '"'],
						J = I[L.category];
						J.count--;
						J.size -= L.size;
					}
					if (_.toString().substring(0, 1) != "\u5206" && !L.isdir) {
						if (C) {
							for (var H in C) {
								if (C.hasOwnProperty(H)) {
									var D = C[H].path;
									if (D.slice(D.indexOf("/")) == F) {
										C.splice(H, 1);
										break;
									}
								}
							}
						}
					}
					if (_.toString().substring(0, 1) == "\u5206") {
						var K = L.path,
						E = K.slice(K.indexOf("/"));
						this.query(E, undefined, function (A, _) {
							_.splice(A, 1);
						});
					}
					A.splice(G, 1);
					this._removeDescendent(F, false);
					break;
				}
			}
		}
		this._mDirty = true;
	}).method("query", function (I, _, H) {
		if (I[I.length - 1] == "/" || I == "0" || I == "") {
			var G = I.slice(0, -1) || "0",
			C = this._mCache[G];
			C && H && H();
			return C;
		} else {
			var F = I.slice(I.lastIndexOf("/") + 1),
			E = I.substring(0, 1) == "/" ? this._mCache[I.slice(0, I.lastIndexOf("/")) || "0"] : this._mCache[I.slice(0, I.indexOf("/"))];
			if (!E) {
				return undefined;
			}
			for (var A = 0, B = E.length; A < B; A++) {
				var D = E[A].path;
				if (D.slice(D.lastIndexOf("/") + 1) == F) {
					C = E[A];
					H && H(A, E);
					return C;
				}
			}
			return undefined;
		}
	}).method("quoteAdd", function (_, H) {
		var B = [];
		for (var A = 0, D = H.length; A < D; A++) {
			var G = H[A].path,
			E = G.slice(G.indexOf(":/") + 1),
			C = this.query(E);
			if (!C) {
				var F = G.slice(G.indexOf(":/") + 1, G.lastIndexOf("/"));
				this._mCache[F] = (this._mCache[F] || []);
				this._pathByType = (this._pathByType || {});
				C = H[A];
				this._mCache[F].push(H[A]);
				this._pathByType[F] = _;
			}
			B.push(C);
		}
		this.add(_, B);
		return this;
	}).method("removeCategorys", function () {
		var A = FileUtils.TYPES;
		for (var _ = 0; _ < 6; _++) {
			var B = '\u5206\u7c7b:"' + A[_][1] + '"';
			this._mCache[B] = null;
			delete this._mCache[B];
		}
		this._typeTable = null;
		this._pathByType = {};
	}).method("contains", function (_) {
		if (this._TimeStamp && this._TimeStamp[_] && (new Date().getTime() - this._TimeStamp[_]) > (Utilities._mInterval || 3600000)) {
			this._mCache[_] = null;
			delete this._mCache[_];
		}
		return typeof this._mCache[_] != "undefined";
	}).method("obtain", function (_) {
		if (this._TimeStamp && this._TimeStamp[_] && (new Date().getTime() - this._TimeStamp[_]) > (Utilities._mInterval || 3600000)) {
			this._mCache[_] = null;
			delete this._mCache[_];
		}
		return this._mCache[_ || "0"] || null;
	});
	disk.util.FileCache = A;
})();
(function () {
	var _ = function (_, A) {
		this._timer = null;
		this._interval = _;
		this._actionListener = A;
		this._isAlive = false;
		this._startTime = 0;
	};
	_.prototype = {
		isAlive : function () {
			return this._isAlive;
		},
		interrupt : function () {
			if (this._timer) {
				clearTimeout(this._timer);
			}
			this._timer = null;
			this._isAlive = false;
			this._startTime = 0;
		},
		setActionListener : function (_) {
			if (typeof _ == "function") {
				this._actionListener = _;
			}
		},
		getActionListener : function () {
			return this._actionListener;
		},
		start : function () {
			var _ = this;
			this._timer = setTimeout(function () {
					_.interrupt();
					_._actionListener.call(_);
				}, this._interval);
			this._startTime = (new Date()).getTime();
			this._isAlive = true;
		},
		getElapsed : function () {
			return (new Date()).getTime() - this._startTime;
		},
		setInterval : function (_) {
			if (typeof _ == "number") {
				this._interval = _;
			}
		},
		getInterval : function () {
			return this._interval;
		}
	};
	disk.util.TimerService = _;
})();
(function () {
	var A = function () {
		this._mIFrame = null;
		this._mState = "";
		this._mTimer = null;
	};
	A.singleton = null;
	A.getDefault = function () {
		if (!A.singleton) {
			A.singleton = new A();
			A.singleton.init();
		}
		return A.singleton;
	};
	A.HISTORY_IFRAME_ID = "historyIFrameEmulator";
	A.prototype = {
		_buildIFrame : function () {
			var _ = document.getElementById(A.HISTORY_IFRAME_ID),
			B = this;
			if (!_) {
				throw new Error("History Iframe Not Found");
			}
			this._mIFrame = _;
		},
		init : function () {
			this._buildIFrame();
			this._startTimer();
			this._addState("");
		},
		dispatchIFrameLoaded : function (_) {
			this._mState = _;
			this.onHistoryChanged(_);
		},
		_startTimer : function () {
			if (!this._mTimer) {
				var _ = this;
				this._mTimer = setInterval(function () {
						if (!_._mIFrame) {
							return;
						}
						try {
							var A = _._mIFrame.contentWindow.document.body.innerText.trim();
							if (A != _._mState) {
								_.dispatchIFrameLoaded(A);
							}
						} catch (B) {}

					}, 50);
			}
		},
		_stopTimer : function () {
			if (this._mTimer) {
				clearInterval(this._mTimer);
				this._mTimer = null;
			}
		},
		onHistoryChanged : function (_) {},
		_addState : function (B) {
			var _ = null,
			A = "<html><body>" + B + "</body></html>";
			try {
				_ = this._mIFrame.contentWindow.document;
				_.open();
				_.writeln(A);
				_.close();
				return true;
			} catch (C) {
				return false;
			}
		},
		replaceHistoryState : function (_) {
			try {
				this._mIFrame.contentWindow.document.body.innerHTML = _;
				return true;
			} catch (A) {
				return false;
			}
		},
		addHistoryState : function (A) {
			var _ = this._addState(A);
			return _;
		}
	};
	disk.util.HistoryStackEmulator = A;
	var _ = function () {
		this._mEmulator = null;
		this._mModules = {};
		this._mLastHistoryState = null;
	};
	_.PATTERN_HISTORY_STATE = /^([\w-]+)((\/(?:[\w-]+)=[^&]+(&(?:[\w-]+)=[^&]+)*)*)$/gi;
	_.parseHistory = function (C) {
		_.PATTERN_HISTORY_STATE.lastIndex = 0;
		if (_.PATTERN_HISTORY_STATE.test(C)) {
			var D = RegExp.$2.trim(),
			G = {};
			if (D.length > 0) {
				for (var E = D.substring(1).split("&"), B = 0, A = E.length; B < A; B++) {
					var F = E[B].split("=");
					G[F[0]] = decodeURIComponent(F[1]);
				}
			}
			return [RegExp.$1.trim(), G];
		} else {
			return null;
		}
	};
	_.assignUrlHash = function (A, _) {
		if (A.trim().length == 0 && _ === true) {
			return;
		}
		try {
			location.hash = A;
		} catch (B) {
			if (disk.DEBUG) {
				console.log("Exception On Assigning Url Hash = ", A);
			}
		}
	};
	_.replaceUrlHash = function (A, _) {
		if (A.trim().length == 0 && _ === true) {
			return;
		}
		try {
			location.replace("#" + A);
		} catch (B) {
			if (disk.DEBUG) {
				console.log("Exception On Assigning Url Hash = ", A);
			}
		}
	};
	_.buildHistory = function (C, _) {
		var A = [];
		for (var B in _) {
			if (_.hasOwnProperty(B)) {
				A.push(B + "=" + encodeURIComponent(_[B]));
			}
		}
		return C + (A.length > 0 ? "/" + A.join("&") : "");
	};
	_.interpretCurrentWindow = function (B) {
		var C = location.href.indexOf("#");
		if (C != -1) {
			var D = location.href.substring(C + 1);
			if (D.length > 0) {
				var A = _.parseHistory(D);
				if (A != null) {
					if (B === true) {
						return A;
					}
					_.getDefault().dispatchHistoryChanged(D);
					return true;
				}
			}
		}
		if (B === true) {
			return null;
		}
		return false;
	};
	_.instance = null;
	_.getDefault = function () {
		if (!_.instance) {
			_.instance = new _();
			_.instance.init();
		}
		return _.instance;
	};
	_.prototype = {
		init : function () {
			var D = (typeof window.attachEvent != "undefined"),
			B = ("onhashchange" in window),
			E = this;
			if (B) {
				var C = /MSIE (\d+)/gi;
				if (C.test(navigator.userAgent)) {
					if (RegExp.$1 < 10) {
						B = false;
					}
				}
			}
			if (D && !B) {
				this._mEmulator = A.getDefault();
				this._mEmulator.onHistoryChanged = function (_) {
					E.dispatchHistoryChanged(_);
				};
			} else {
				var _ = function () {
					if (disk.DEBUG) {
						console.log("[LOG]new hash = ", location.hash);
					}
					var _ = location.href.indexOf("#"),
					A = "";
					if (_ != -1) {
						A = location.href.substring(_ + 1);
					}
					E.dispatchHistoryChanged(A);
				};
				if (typeof window.addEventListener != "undefined") {
					window.addEventListener("hashchange", _, false);
				} else {
					if (typeof window.attachEvent != "undefined") {
						window.attachEvent("onhashchange", _);
					}
				}
			}
		},
		listen : function (_, A) {
			this._mModules[_] = A;
		},
		unlisten : function (_) {
			delete this._mModules[_];
		},
		addHistory : function (_) {
			_ = _.trim();
			if (_.length > 0) {
				disk.util.HistoryManager.assignUrlHash(_);
				if (this._mEmulator != null) {
					this._mEmulator.addHistoryState(_);
				}
			}
		},
		replaceHistory : function (_) {
			_ = _.trim();
			if (_.length > 0) {
				disk.util.HistoryManager.replaceUrlHash(_);
				if (this._mEmulator != null) {
					this._mEmulator.replaceHistoryState(_);
				}
			}
		},
		dispatchHistoryChanged : function (C) {
			if (this._mEmulator != null) {
				disk.util.HistoryManager.assignUrlHash(C);
			}
			this._mLastHistoryState = C;
			if (C.trim().length == 0) {
				if (disk.DEBUG) {
					console.log(">>>>>notify all clients");
				}
				for (var A in this._mModules) {
					if (this._mModules.hasOwnProperty(A)) {
						this._mModules[A].call(this, {}, true);
					}
				}
			} else {
				var B = _.parseHistory(C);
				if (B != null) {
					A = this._mModules[B[0]];
					if (A) {
						A.call(this, B[1]);
					}
				}
			}
		},
		getCurrentHistoryState : function () {
			return this._mLastHistoryState;
		},
		navigate : function (D, A, C) {
			if (typeof D == "undefined" || D == null) {
				throw new Error("Arguments 'module' required to navigate history");
			}
			var B = _.buildHistory(D, A);
			if (B == this._mLastHistoryState) {
				return false;
			}
			if (C === true) {
				this.replaceHistory(B);
			} else {
				this.addHistory(B);
			}
			return true;
		},
		go : function (_) {
			history.go(_);
		}
	};
	disk.util.HistoryManager = _;
})();
disk.util.ErrorMessage = {
	"0" : "\u6210\u529f",
	"-1" : "\u7528\u6237\u540d\u548c\u5bc6\u7801\u9a8c\u8bc1\u5931\u8d25",
	"-2" : "\u5907\u7528",
	"-3" : "\u7528\u6237\u672a\u6fc0\u6d3b\uff08\u8c03\u7528init\u63a5\u53e3\uff09",
	"-4" : "COOKIE\u4e2d\u672a\u627e\u5230host_key&user_key\uff08\u6216BDUSS\uff09",
	"-5" : "host_key\u548cuser_key\u65e0\u6548",
	"-6" : "bduss\u65e0\u6548",
	"-7" : "\u6587\u4ef6\u6216\u76ee\u5f55\u540d\u9519\u8bef\u6216\u65e0\u6743\u8bbf\u95ee",
	"-8" : "\u8be5\u76ee\u5f55\u4e0b\u5df2\u5b58\u5728\u6b64\u6587\u4ef6",
	"-9" : "\u6587\u4ef6\u88ab\u6240\u6709\u8005\u5220\u9664\uff0c\u64cd\u4f5c\u5931\u8d25",
	"-10" : "\u7f51\u76d8\u7a7a\u95f4\u5df2\u6ee1",
	"-11" : "\u7236\u76ee\u5f55\u4e0d\u5b58\u5728",
	"-12" : "\u8bbe\u5907\u5c1a\u672a\u6ce8\u518c",
	"-13" : "\u8bbe\u5907\u5df2\u7ecf\u88ab\u7ed1\u5b9a",
	"-14" : "\u5e10\u53f7\u5df2\u7ecf\u521d\u59cb\u5316",
	"-21" : "\u9884\u7f6e\u6587\u4ef6\u65e0\u6cd5\u8fdb\u884c\u76f8\u5173\u64cd\u4f5c",
	"-22" : "\u88ab\u5206\u4eab\u7684\u6587\u4ef6\u65e0\u6cd5\u91cd\u547d\u540d\uff0c\u79fb\u52a8\u7b49\u64cd\u4f5c",
	"-23" : "\u6570\u636e\u5e93\u64cd\u4f5c\u5931\u8d25\uff0c\u8bf7\u8054\u7cfbnetdisk\u7ba1\u7406\u5458",
	"-24" : "\u8981\u53d6\u6d88\u7684\u6587\u4ef6\u5217\u8868\u4e2d\u542b\u6709\u4e0d\u5141\u8bb8\u53d6\u6d88public\u7684\u6587\u4ef6\u3002",
	"-25" : "\u975e\u516c\u6d4b\u7528\u6237",
	"-26" : "\u9080\u8bf7\u7801\u5931\u6548",
	"1" : "\u670d\u52a1\u5668\u9519\u8bef ",
	"2" : "\u8be5\u6587\u4ef6\u5939\u4e0d\u53ef\u4ee5\u79fb\u52a8",
	"3" : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc7100\u4e2a",
	"4" : "\u65b0\u6587\u4ef6\u540d\u9519\u8bef",
	"5" : "\u76ee\u6807\u76ee\u5f55\u975e\u6cd5",
	"6" : "\u5907\u7528",
	"7" : "NS\u975e\u6cd5\u6216\u65e0\u6743\u8bbf\u95ee",
	"8" : "ID\u975e\u6cd5\u6216\u65e0\u6743\u8bbf\u95ee",
	"9" : "\u7533\u8bf7key\u5931\u8d25",
	"10" : "\u521b\u5efa\u6587\u4ef6\u7684superfile\u5931\u8d25",
	"11" : "user_id(\u6216user_name)\u975e\u6cd5\u6216\u4e0d\u5b58\u5728",
	"12" : "\u6279\u91cf\u5904\u7406\u672a\u5168\u90e8\u6210\u529f",
	"13" : "\u6b64\u76ee\u5f55\u65e0\u6cd5\u5171\u4eab",
	"14" : "\u7cfb\u7edf\u9519\u8bef",
	"103" : "\u63d0\u53d6\u7801\u9519\u8bef",
	"104" : "\u9a8c\u8bc1cookie\u65e0\u6548",
	"201" : "\u7cfb\u7edf\u9519\u8bef",
	"202" : "\u7cfb\u7edf\u9519\u8bef",
	"203" : "\u7cfb\u7edf\u9519\u8bef",
	"204" : "\u7cfb\u7edf\u9519\u8bef",
	"205" : "\u7cfb\u7edf\u9519\u8bef",
	"301" : "\u5176\u4ed6\u8bf7\u6c42\u51fa\u9519",
	"501" : "\u83b7\u53d6\u7684LIST\u683c\u5f0f\u975e\u6cd5",
	"618" : "\u8bf7\u6c42curl\u8fd4\u56de\u5931\u8d25",
	"619" : "pcs\u8fd4\u56de\u9519\u8bef\u7801",
	"600" : "json\u89e3\u6790\u51fa\u9519",
	"601" : "exception\u629b\u51fa\u5f02\u5e38",
	"617" : "getFilelist\u5176\u4ed6\u9519\u8bef",
	"211" : "\u65e0\u6743\u64cd\u4f5c\u6216\u88ab\u5c01\u7981",
	"404" : "\u79d2\u4f20md5\u4e0d\u5339\u914d rapidupload \u9519\u8bef\u7801",
	"406" : "\u79d2\u4f20\u521b\u5efa\u6587\u4ef6\u5931\u8d25 rapidupload \u9519\u8bef\u7801",
	"407" : "fileModify\u63a5\u53e3\u8fd4\u56de\u9519\u8bef\uff0c\u672a\u8fd4\u56derequestid rapidupload \u9519\u8bef\u7801",
	"31080" : "\u6211\u4eec\u7684\u670d\u52a1\u5668\u51fa\u9519\u4e86\uff0c\u7a0d\u5019\u8bd5\u8bd5\u5427",
	"31021" : "\u7f51\u7edc\u8fde\u63a5\u5931\u8d25\uff0c\u8bf7\u68c0\u67e5\u7f51\u7edc\u6216\u7a0d\u5019\u518d\u8bd5",
	"31075" : "\u4e00\u6b21\u652f\u6301\u64cd\u4f5c999\u4e2a\uff0c\u51cf\u70b9\u8bd5\u8bd5\u5427",
	"31116" : "\u4f60\u7684\u7a7a\u95f4\u4e0d\u8db3\u4e86\u54df\uff0c\u8d76\u7d27<a target='_blank' href='http://yun.baidu.com/buy/center?tag=2'>\u8d2d\u4e70\u7a7a\u95f4</a>\u5427"
};
disk.util.shareErrorMessage = {
	"0" : "\u6210\u529f",
	"-1" : "\u7531\u4e8e\u60a8\u5206\u4eab\u4e86\u8fdd\u53cd\u76f8\u5173\u6cd5\u5f8b\u6cd5\u89c4\u7684\u6587\u4ef6\uff0c\u5206\u4eab\u529f\u80fd\u5df2\u88ab\u7981\u7528\uff0c\u4e4b\u524d\u5206\u4eab\u51fa\u53bb\u7684\u6587\u4ef6\u4e0d\u53d7\u5f71\u54cd\u3002",
	"-2" : "\u7528\u6237\u4e0d\u5b58\u5728,\u8bf7\u5237\u65b0\u9875\u9762\u540e\u91cd\u8bd5",
	"-3" : "\u6587\u4ef6\u4e0d\u5b58\u5728,\u8bf7\u5237\u65b0\u9875\u9762\u540e\u91cd\u8bd5",
	"-4" : "\u767b\u5f55\u4fe1\u606f\u6709\u8bef\uff0c\u8bf7\u91cd\u65b0\u767b\u5f55\u8bd5\u8bd5",
	"-5" : "host_key\u548cuser_key\u65e0\u6548",
	"-6" : "\u8bf7\u91cd\u65b0\u767b\u5f55",
	"-7" : "\u8be5\u5206\u4eab\u5df2\u5220\u9664\u6216\u5df2\u53d6\u6d88",
	"-8" : "\u8be5\u5206\u4eab\u5df2\u7ecf\u8fc7\u671f",
	"-9" : "\u8bbf\u95ee\u5bc6\u7801\u9519\u8bef",
	"-10" : "\u5206\u4eab\u5916\u94fe\u5df2\u7ecf\u8fbe\u5230\u6700\u5927\u4e0a\u9650100000\u6761\uff0c\u4e0d\u80fd\u518d\u6b21\u5206\u4eab",
	"-11" : "\u9a8c\u8bc1cookie\u65e0\u6548",
	"-14" : "\u5bf9\u4e0d\u8d77\uff0c\u77ed\u4fe1\u5206\u4eab\u6bcf\u5929\u9650\u523620\u6761\uff0c\u4f60\u4eca\u5929\u5df2\u7ecf\u5206\u4eab\u5b8c\uff0c\u8bf7\u660e\u5929\u518d\u6765\u5206\u4eab\u5427\uff01",
	"-15" : "\u5bf9\u4e0d\u8d77\uff0c\u90ae\u4ef6\u5206\u4eab\u6bcf\u5929\u9650\u523620\u5c01\uff0c\u4f60\u4eca\u5929\u5df2\u7ecf\u5206\u4eab\u5b8c\uff0c\u8bf7\u660e\u5929\u518d\u6765\u5206\u4eab\u5427\uff01",
	"-16" : "\u5bf9\u4e0d\u8d77\uff0c\u8be5\u6587\u4ef6\u5df2\u7ecf\u9650\u5236\u5206\u4eab\uff01",
	"-17" : "\u6587\u4ef6\u5206\u4eab\u8d85\u8fc7\u9650\u5236",
	"-30" : "\u6587\u4ef6\u5df2\u5b58\u5728",
	"-31" : "\u6587\u4ef6\u4fdd\u5b58\u5931\u8d25",
	"-33" : "\u4e00\u6b21\u652f\u6301\u64cd\u4f5c10000\u4e2a\uff0c\u51cf\u70b9\u8bd5\u8bd5\u5427",
	"-32" : "\u4f60\u7684\u7a7a\u95f4\u4e0d\u8db3\u4e86\u54df\uff0c\u8d76\u7d27<a target='_blank' href='http://yun.baidu.com/buy/center?tag=2'>\u8d2d\u4e70\u7a7a\u95f4</a>\u5427",
	"-70" : "\u4f60\u5206\u4eab\u7684\u6587\u4ef6\u4e2d\u5305\u542b\u75c5\u6bd2\u6216\u7591\u4f3c\u75c5\u6bd2\uff0c\u4e3a\u4e86\u4f60\u548c\u4ed6\u4eba\u7684\u6570\u636e\u5b89\u5168\uff0c\u6362\u4e2a\u6587\u4ef6\u5206\u4eab\u5427",
	"2" : "\u53c2\u6570\u9519\u8bef",
	"3" : "\u672a\u767b\u5f55\u6216\u5e10\u53f7\u65e0\u6548",
	"4" : "\u5b58\u50a8\u597d\u50cf\u51fa\u95ee\u9898\u4e86\uff0c\u8bf7\u7a0d\u5019\u518d\u8bd5",
	"108" : "\u6587\u4ef6\u540d\u6709\u654f\u611f\u8bcd\uff0c\u4f18\u5316\u4e00\u4e0b\u5427",
	"110" : "\u5206\u4eab\u6b21\u6570\u8d85\u51fa\u9650\u5236\uff0c\u53ef\u4ee5\u5230\u201c\u6211\u7684\u5206\u4eab\u201d\u4e2d\u67e5\u770b\u5df2\u5206\u4eab\u7684\u6587\u4ef6\u94fe\u63a5",
	"114" : "\u5f53\u524d\u4efb\u52a1\u4e0d\u5b58\u5728\uff0c\u4fdd\u5b58\u5931\u8d25",
	"115" : "\u8be5\u6587\u4ef6\u7981\u6b62\u5206\u4eab"
};
disk.util.downloadMessage = {
	"36000" : "\u7f51\u7edc\u7e41\u5fd9\uff0c\u8bf7\u7a0d\u5019\u518d\u8bd5",
	"36001" : "\u53c2\u6570\u9519\u8bef",
	"36002" : "appid\u9519\u8bef",
	"36003" : "\u8bf7\u5237\u65b0\u518d\u8bd5",
	"36004" : "\u8bf7\u91cd\u65b0\u767b\u5f55",
	"36005" : "\u7528\u6237\u672a\u767b\u5f55",
	"36006" : "\u7528\u6237\u672a\u6fc0\u6d3b",
	"36007" : "\u7528\u6237\u672a\u6388\u6743",
	"36008" : "\u7528\u6237\u4e0d\u5b58\u5728",
	"36009" : "\u7528\u6237\u7a7a\u95f4\u4e0d\u8db3",
	"36010" : "\u6587\u4ef6\u4e0d\u5b58\u5728",
	"36012" : "\u64cd\u4f5c\u8d85\u65f6\uff0c\u8bf7\u91cd\u8bd5",
	"36013" : "\u540c\u65f6\u4e0b\u8f7d\u7684\u4efb\u52a1\u8fc7\u591a\uff0c\u4e0d\u80fd\u4e0b\u8f7d",
	"36014" : "\u5b58\u50a8\u8def\u5f84\u5df2\u4f7f\u7528",
	"36016" : "\u4efb\u52a1\u5df2\u5220\u9664",
	"36017" : "\u4efb\u52a1\u5df2\u5b8c\u6210",
	"36018" : "\u89e3\u6790\u5931\u8d25\uff0c\u79cd\u5b50\u6587\u4ef6\u635f\u574f",
	"36019" : "\u4efb\u52a1\u6b63\u5728\u5904\u7406\u4e2d",
	"36020" : "\u4efb\u52a1\u5730\u5740\u4e0d\u5b58\u5728",
	"36021" : "\u666e\u901a\u7528\u6237\u6700\u591a\u540c\u65f6\u4e0b\u8f7d1\u4e2a\u4efb\u52a1\u54e6\uff01\u9a6c\u4e0a\u5f00\u901a\u79bb\u7ebf\u4e0b\u8f7d\u5957\u9910\uff0c\u7acb\u5373\u4e0b\u8f7d\u66f4\u591a\uff01",
	"36022" : "\u540c\u65f6\u4e0b\u8f7d\u7684\u4efb\u52a1\u8fc7\u591a\uff0c\u4e0d\u80fd\u4e0b\u8f7d",
	"36023" : "\u666e\u901a\u7528\u6237\u6bcf\u6708\u53ea\u80fd\u79bb\u7ebf\u4e0b\u8f7d5\u4e2a\u4efb\u52a1\u54e6\uff01\u9a6c\u4e0a\u5f00\u901a\u79bb\u7ebf\u4e0b\u8f7d\u5957\u9910\uff0c\u7acb\u5373\u4e0b\u8f7d\u66f4\u591a\uff01",
	"36024" : "\u672c\u6708\u4e0b\u8f7d\u6570\u5df2\u8d85\u9650\u5236",
	"36025" : "\u5206\u4eab\u94fe\u63a5\u5df2\u5931\u6548",
	"36026" : "\u94fe\u63a5\u683c\u5f0f\u6709\u8bef",
	"36027" : "\u94fe\u63a5\u683c\u5f0f\u6709\u8bef",
	"36028" : "\u6682\u65f6\u65e0\u6cd5\u627e\u5230\u76f8\u5173\u79cd\u5b50\u4fe1\u606f",
	"36031" : "\u7f51\u7edc\u7e41\u5fd9\uff0c\u8bf7\u7a0d\u5019\u518d\u8bd5",
	"-19" : "\u8bf7\u8f93\u5165\u9a8c\u8bc1\u7801"
};
disk.util.buyErrorMessage = {
	"1000" : "\u554a\u54e6\uff0c\u4e0d\u80fd\u4ece\u9ad8\u7ea7\u5957\u9910\u5347\u7ea7\u5230\u4f4e\u7ea7\u5957\u9910\uff0c\u8bf7\u91cd\u65b0\u8d2d\u4e70\u5427\u3002",
	"1001" : "\u554a\u54e6\uff0c\u4e0d\u80fd\u4ece\u9ad8\u7ea7\u5957\u9910\u5347\u7ea7\u5230\u4f4e\u7ea7\u5957\u9910\uff0c\u8bf7\u91cd\u65b0\u8d2d\u4e70\u5427\u3002",
	"1002" : "\u554a\u54e6\uff0c\u521b\u5efa\u8ba2\u5355\u5931\u8d25\u4e86\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5\u4e00\u4e0b\u5427\u3002",
	"1003" : "\u554a\u54e6\uff0c\u521b\u5efa\u8ba2\u5355\u5931\u8d25\u4e86\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5\u4e00\u4e0b\u5427\u3002",
	"1004" : "\u554a\u54e6\uff0c\u521b\u5efa\u8ba2\u5355\u5931\u8d25\u4e86\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5\u4e00\u4e0b\u5427\u3002",
	"1005" : "\u554a\u54e6\uff0c\u521b\u5efa\u8ba2\u5355\u5931\u8d25\u4e86\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5\u4e00\u4e0b\u5427\u3002",
	"1006" : "\u554a\u54e6\uff0c\u4e0d\u80fd\u540c\u4e00\u5929\u8d2d\u4e70\u76f8\u540c\u4ea7\u54c1\uff0c\u8bf7\u91cd\u65b0\u8d2d\u4e70\u5427\u3002",
	"1007" : "\u554a\u54e6\uff0c\u4e0d\u80fd\u540c\u4e00\u5929\u8d2d\u4e70\u76f8\u540c\u4ea7\u54c1\uff0c\u8bf7\u91cd\u65b0\u8d2d\u4e70\u5427\u3002",
	"3002" : "\u554a\u54e6\uff0c\u8be5\u8ba2\u5355\u5df2\u7ecf\u652f\u4ed8\u6210\u529f\uff0c\u8bf7\u91cd\u65b0\u8d2d\u4e70\u5427\u3002",
	"3003" : "\u554a\u54e6\uff0c\u8be5\u8ba2\u5355\u5df2\u7ecf\u652f\u4ed8\u5931\u8d25\uff0c\u8bf7\u91cd\u65b0\u8d2d\u4e70\u5427\u3002",
	"36000" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36001" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36002" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36003" : "\u8be5\u6b21\u8bbf\u95ee\u5f02\u5e38\uff0c\u6743\u9650\u53d7\u9650",
	"36005" : "\u9a8c\u8bc1\u7801\u8f93\u5165\u9519\u8bef\uff0c\u8bf7\u5237\u65b0\u91cd\u8bd5",
	"36006" : "\u554a\u54e6\uff0c\u6211\u4eec\u8fd8\u6ca1\u63a8\u51fa\u8fd9\u6b3e\u4ea7\u54c1\u5440",
	"36007" : "\u4f5c\u5f02\u5e38\uff0c\u559d\u676f\u8336\u91cd\u8bd5\u4e0b\uff1f",
	"36008" : "\u54ce\u5440\uff0c\u4f60\u7684\u64cd\u4f5c\u5f02\u5e38\uff0c\u8bf7\u5237\u65b0\u91cd\u8bd5\u3002",
	"36009" : "\u57fa\u7840\u5957\u9910\u4e0d\u5141\u8bb8\u8d2d\u4e70",
	"36010" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36011" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36012" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36013" : "\u8be5\u8ba2\u5355\u65e0\u6cd5\u91cd\u65b0\u652f\u4ed8",
	"36014" : "\u554a\u54e6\uff0c\u8be5\u8ba2\u5355\u5df2\u7ecf\u652f\u4ed8\u6210\u529f\uff0c\u8bf7\u91cd\u65b0\u8d2d\u4e70\u5427\u3002",
	"36015" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36016" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36017" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36018" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026",
	"36019" : "\u554a\u54e6\uff0c\u670d\u52a1\u5668\u611f\u5192\u4e86\uff0c\u7d27\u6025\u533b\u6cbb\u4e2d\u2026\u2026"
};
disk.util.recordErrorMessage = {
	"36000" : "\u5185\u90e8\u9519\u8bef",
	"36001" : "\u4e0d\u652f\u6301\u7684API",
	"36002" : "\u53c2\u6570\u9519\u8bef",
	"36003" : "\u65e0\u6743\u9650\u8bbf\u95ee",
	"36005" : "\u9a8c\u8bc1\u7801\u5931\u6548\u6216\u975e\u6cd5",
	"36006" : "\u8981\u8d2d\u4e70\u7684\u4ea7\u54c1\u4e0d\u5b58\u5728",
	"36007" : "\u7528\u6237\u5728\u8d2d\u4e70\u4ea7\u54c1\u7684\u8fc7\u7a0b\u4e2d\u5e76\u884c\u64cd\u4f5c\uff0c\u4e00\u822c\u4e0d\u4f1a\u51fa\u73b0\u8be5\u9519\u8bef\u7801",
	"36016" : "\u8be5\u529f\u80fd\u4e0d\u662f\u5c5e\u6027\u503c\uff0c\u4e0d\u80fd\u83b7\u53d6\uff0c\u63a5\u5165\u7cfb\u7edf\u5173\u6ce8",
	"36017" : "\u8be5\u529f\u80fd\u4e0d\u662f\u6d88\u8d39\u503c\uff0c\u4e0d\u80fd\u6d88\u8d39\uff0c\u63a5\u5165\u7cfb\u7edf\u5173\u6ce8",
	"36018" : "\u8981\u6d88\u8d39\u7684\u529f\u80fd\u9879\u672a\u627e\u5230\uff0c\u63a5\u5165\u7cfb\u7edf\u5173\u6ce8",
	"36019" : "\u62d2\u7edd\u6d88\u8d39\uff0c\u7528\u6237\u6ca1\u6709\u914d\u989d\u4e86\uff0c\u63a5\u5165\u7cfb\u7edf\u5173\u6ce8",
	"36020" : "\u8bf7\u6c42\u91cd\u653e",
	"36021" : "\u8bf7\u6c42\u8fc7\u671f\uff0c\u6216\u8005\u662f\u7b2c\u4e09\u65b9\u4f2a\u9020\u7684\u8bf7\u6c42",
	"36031" : "\u7b2c\u4e09\u65b9api\u53c2\u6570\u9519\u8bef",
	"36032" : "\u7b2c\u4e09\u65b9api\u7b7e\u540d\u9519\u8bef",
	"36033" : "\u7b2c\u4e09\u65b9api\u6587\u4ef6\u9519\u8bef",
	"36034" : "\u6570\u636e\u5e93\u9519\u8bef",
	"36035" : "\u8ba2\u5355\u5df2\u7ecf\u5b58\u5728",
	"36036" : "\u8ba2\u5355token\u5931\u6548",
	"36037" : "\u67e5\u8be2\u8ba2\u5355\u4e0d\u5b58\u5728",
	"36038" : "\u7701\u5e02\u53c2\u6570\u9519\u8bef",
	"36039" : "\u5355\u4ef7\u4e0d\u5b58\u5728"
};
(function () {
	var _ = function (_) {
		this._mRawMsgMap = _ || {};
	};
	_.PATTERN = /%((?:\d+\$)*)((?:\.\d+)*)(d|f|s|t|T|\%)/gi;
	_.sprintf = function () {
		if (arguments.length == 0) {
			throw new Error("{template} required to format string");
		}
		if (arguments.length == 1) {
			return arguments[0];
		}
		var E = [],
		B = null,
		F = arguments[0],
		C = 0,
		G = 0,
		H = null,
		D = null;
		while ((B = _.PATTERN.exec(F)) != null) {
			E.push(F.substring(G, B.index));
			H = B[3];
			D = B[1].length == 0 ? arguments[C + 1] : arguments[parseInt(B[1].substring(0, B[1].length - 1))];
			if (typeof D == "undefined") {
				D = "";
			}
			if (H == "%") {
				var A = parseFloat(D);
				E.push(isNaN(A) ? B[0] : A * 100 + "%");
			} else {
				if (H.toLowerCase() == "d") {
					E.push(parseInt(D));
				} else {
					if (H.toLowerCase() == "f") {
						A = parseFloat(D);
						if (B[2].length > 0) {
							A = A.toFixed(parseInt(B[2].substring(1)));
						}
						E.push(isNaN(A) ? B[0] : A);
					} else {
						if (H.toLowerCase() == "s") {
							E.push(D);
						} else {
							if (H.toLowerCase() == "t") {
								if (D instanceof Date) {
									E.push(H == "t" ? D.getTime() : D);
								} else {
									E.push(B[0]);
								}
							}
						}
					}
				}
			}
			C++;
			G = B.index + B[0].length;
		}
		if (G < F.length - 1) {
			E.push(F.substring(G));
		}
		return E.join("");
	};
	_.prototype = {
		getMessage : function () {
			if (arguments.length == 0) {
				throw new Error("{errno} required to retrive message content");
			}
			var D = this._mRawMsgMap[arguments[0]];
			if (typeof D == "undefined") {
				return null;
			}
			var C = [D];
			for (var B = 1, A = arguments.length; B < A; B++) {
				C.push(arguments[B]);
			}
			return _.sprintf.apply(_.sprintf, C);
		},
		getMessageMap : function () {
			return this._mRawMsgMap;
		}
	};
	disk.util.MessageFormatter = _;
})();
(function () {
	var _ = function (A, C, B, _) {
		this._mRestfulDataUri = A;
		this._mRequestMethod = C;
		this._mQueryArgs = B || [];
		this._mRequestData = _;
		this._mPage = 0;
		this._mElementsData = [];
		this._mMessageFormatter = null;
		this._mTag = null;
		this._mXHR = null;
		this._mPrivateFlags = 0;
		this._mEND_SHOW_NOTIFY = false;
		this._mIsLastPage = false;
	};
	_.NEED_LOAD_MORE_PAGE = 1;
	_.CACHE_LIST_DATA = 2;
	_.LOADED_AT_LEAST_ONCE = 4;
	_.LOADING = 8;
	_.NOT_IDLE = 16;
	_.PAGING_BASE_ONE = 32;
	_.ABORT_MANULLY = 64;
	_.HTTP_GET = 0;
	_.HTTP_POST = 1;
	_.HTTP_KEY_EXCEPTION = -1;
	_.DATA_KEY_ERRNO = "errno";
	_.DATA_KEY_LIST = "list";
	_.DATA_KEY_INFO = "info";
	_.DATA_KEY_TOTAL = "total";
	_.QUERY_KEY_PAGE = "page";
	_.QUERY_KEY_TIME = "t";
	_.getColumnByKey = function (C, _, B) {
		var A = C[_];
		if (typeof A == "undefined") {
			return typeof B == "undefined" ? null : B;
		} else {
			return A;
		}
	};
	_.appendQueryParam = function (B, _, A) {
		if (B.length == 0) {
			throw new Error("baseUrl required for composing url");
		}
		var C = B.indexOf("?") == -1 ? "?" : "&";
		return B + C + _ + "=" + A;
	};
	_.HTTP_TIME_OUT = 30 * 1000;
	_.prototype = {
		isLastPage : function () {
			return this._mIsLastPage;
		},
		setIsLastPage : function (_) {
			if (typeof _ != "undefined" && typeof _ == "boolean") {
				this._mIsLastPage = _;
			} else {
				this._mIsLastPage = false;
			}
		},
		endShowMsg : function (_) {
			if (typeof _ != "undefined" && typeof _ == "boolean") {
				this._mEND_SHOW_NOTIFY = _;
				return;
			}
			return this._mEND_SHOW_NOTIFY;
		},
		onMessagePrewarmed : function (_, A) {
			return _.getMessage(A);
		},
		setMessageFormatter : function (_) {
			this._mMessageFormatter = _;
		},
		getMessageFormatter : function () {
			return this._mMessageFormatter;
		},
		getRequestRawUrl : function () {
			return this._mRestfulDataUri;
		},
		setPagingBaseOne : function (A) {
			if (A) {
				this._mPrivateFlags |= _.PAGING_BASE_ONE;
			} else {
				this._mPrivateFlags &= ~_.PAGING_BASE_ONE;
			}
		},
		setNeedLoadMorePage : function (A) {
			if (A) {
				this._mPrivateFlags |= _.NEED_LOAD_MORE_PAGE;
				this.setIsLastPage(false);
			} else {
				this._mPrivateFlags &= ~_.NEED_LOAD_MORE_PAGE;
				this.setIsLastPage(true);
			}
			this.setCacheListData(A);
		},
		hasMorePage : function () {
			return (this._mPrivateFlags & _.NEED_LOAD_MORE_PAGE) == _.NEED_LOAD_MORE_PAGE;
		},
		setCacheListData : function (A) {
			if (A) {
				this._mPrivateFlags |= _.CACHE_LIST_DATA;
			} else {
				this._mPrivateFlags &= ~_.CACHE_LIST_DATA;
			}
		},
		isCacheListDataEnabled : function () {
			return (this._mPrivateFlags & _.CACHE_LIST_DATA) == _.CACHE_LIST_DATA;
		},
		getCachedListData : function () {
			return this._mElementsData;
		},
		onDataAvailable : function (_, A) {},
		onDataUnavailable : function (_, A) {},
		onLoadingEvent : function (_) {},
		isLoading : function () {
			return (this._mPrivateFlags & _.LOADING) == _.LOADING;
		},
		dispatchLoadingEvent : function () {
			this.onLoadingEvent(this.isLoading());
		},
		dispatchDataUnavailable : function () {
			this._mXHR = null;
			this._mPrivateFlags &= ~_.LOADING;
			this._mPrivateFlags &= ~_.NEED_LOAD_MORE_PAGE;
			this._mPrivateFlags &= ~_.NOT_IDLE;
			this.dispatchLoadingEvent();
			this.setIsLastPage(true);
			var A = false;
			if ((this._mPrivateFlags & _.ABORT_MANULLY) == _.ABORT_MANULLY) {
				this._mPrivateFlags &= ~_.ABORT_MANULLY;
				A = true;
			}
			this.onDataUnavailable(_.HTTP_KEY_EXCEPTION, this._mMessageFormatter, A);
		},
		dispatchDataAvailable : function (B) {
			this._mXHR = null;
			this._mPrivateFlags &= ~_.LOADING;
			this.dispatchLoadingEvent();
			if (B == null) {
				this.dispatchDataUnavailable();
			} else {
				var C = _.getColumnByKey(B, _.DATA_KEY_ERRNO, 0);
				if (C != 0) {
					this._mPrivateFlags &= ~_.NEED_LOAD_MORE_PAGE;
					this._mPrivateFlags &= ~_.NOT_IDLE;
					this.setIsLastPage(true);
					this.onDataUnavailable(C, this._mMessageFormatter);
				} else {
					this._mPrivateFlags |= _.LOADED_AT_LEAST_ONCE;
					var A = this._cacheListData(B);
					this._mPage++;
					this.onDataAvailable(A, this._mElementsData);
				}
			}
		},
		isIdle : function () {
			return (this._mPrivateFlags & _.NOT_IDLE) != _.NOT_IDLE;
		},
		_cacheListData : function (C) {
			var A = _.getColumnByKey(C, _.DATA_KEY_LIST);
			if (A == null) {
				A = _.getColumnByKey(C, _.DATA_KEY_INFO);
			}
			if (A != null) {
				if (A.length == 0) {
					this._mPrivateFlags &= ~_.NEED_LOAD_MORE_PAGE;
					this._mPrivateFlags &= ~_.NOT_IDLE;
					this.setIsLastPage(true);
					return A;
				}
				if (this.isCacheListDataEnabled()) {
					this._mElementsData = this._mElementsData.concat(A);
				}
				var B = _.getColumnByKey(C, _.DATA_KEY_TOTAL, 0);
				if (B > 0 && this._mElementsData.length >= B) {
					this._mPrivateFlags &= ~_.NEED_LOAD_MORE_PAGE;
					this._mPrivateFlags &= ~_.NOT_IDLE;
					this.setIsLastPage(true);
				}
			}
			return A;
		},
		setTag : function (_) {
			this._mTag = _;
		},
		getTag : function () {
			return this._mTag;
		},
		load : function (A) {
			this._mPrivateFlags &= ~_.ABORT_MANULLY;
			var B = false;
			if ((this._mPrivateFlags & _.LOADED_AT_LEAST_ONCE) != _.LOADED_AT_LEAST_ONCE) {
				B = true;
			} else {
				if ((this._mPrivateFlags & _.NEED_LOAD_MORE_PAGE) == _.NEED_LOAD_MORE_PAGE) {
					B = true;
				}
			}
			if (B) {
				this._mPrivateFlags |= _.LOADING;
				this._mPrivateFlags |= _.NOT_IDLE;
				this.dispatchLoadingEvent();
				var C = (typeof A != "undefined") ? A : this._mQueryArgs;
				this._sendHttpRequest(this._mRequestMethod, C, this._mRequestData);
				return true;
			}
			return false;
		},
		setRequestArgReplacements : function (_) {
			this._mQueryArgs = _;
		},
		setRequestData : function (_) {
			this._mRequestData = _;
		},
		reset : function () {
			if (this.isIdle()) {
				this._mElementsData = [];
			} else {
				this._mElementsData.length = 0;
			}
			this._mPrivateFlags &= ~_.LOADED_AT_LEAST_ONCE;
			this._mPage = 0;
			this.abort();
		},
		getLoadedPage : function () {
			return this._mPage;
		},
		abort : function () {
			this._mPrivateFlags &= ~_.LOADING;
			this._mPrivateFlags &= ~_.NOT_IDLE;
			if (this._mXHR) {
				this._mPrivateFlags |= _.ABORT_MANULLY;
				this._mXHR.abort();
				this._mXHR = null;
			}
		},
		clear : function () {
			this._mPrivateFlags = 0;
			this._mPage = 0;
			this._mElementsData.length = 0;
			this._mQueryArgs.length = 0;
			this._mRequestData = null;
		},
		_sendHttpRequest : function (F, A, D) {
			var C = [this._mRestfulDataUri].concat(A),
			E = disk.util.MessageFormatter.sprintf.apply(disk.util.MessageFormatter.sprintf, C, this._mQueryArgs);
			E = _.appendQueryParam(E, _.QUERY_KEY_TIME, new Date().getTime());
			if ((this._mPrivateFlags & _.NEED_LOAD_MORE_PAGE) == _.NEED_LOAD_MORE_PAGE) {
				var B = this._mPage;
				if ((this._mPrivateFlags & _.PAGING_BASE_ONE) == _.PAGING_BASE_ONE) {
					B += 1;
				}
				E = _.appendQueryParam(E, _.QUERY_KEY_PAGE, B);
			}
			var G = this;
			this._mXHR = $.ajax({
					timeout : _.HTTP_TIME_OUT,
					url : E,
					cache : false,
					type : (F == _.HTTP_POST) ? "post" : "get",
					dataType : "json",
					data : D,
					async : true,
					error : function (_, B, A) {
						G.dispatchDataUnavailable();
					},
					success : function (_, A) {
						G.dispatchDataAvailable(_);
					}
				});
		}
	};
	disk.util.RestfulDataLoader = _;
})();
(function () {
	var _ = function () {};
	$(document).ajaxSend(function (D, A, C) {
		if ("script" === C.dataType) {
			return;
		}
		var B = C.url || "";
		if (/\?/.test(B)) {
			if (B.indexOf(_.VENDOR_INFO) == -1) {
				B = B + "&" + _.VENDOR_INFO;
			}
		} else {
			B = B + "?" + _.VENDOR_INFO;
		}
		if ((!/bdstoken\=[^\&]+/.test(B)) && typeof FileUtils !== "undefined" && typeof FileUtils.bdstoken !== "undefined") {
			B = B + "&bdstoken=" + FileUtils.bdstoken;
		}
		C.url = B;
	});
	_.normalize = function (A, D) {
		if (typeof A != "string" || A.length == 0) {
			return A;
		}
		if (D == null || typeof D != "string") {
			return A;
		}
		var C = A.indexOf("?") != -1,
		E = A.substring(A.length - 1),
		_ = "bdstoken=" + D,
		B;
		if (E == "&" || E == "?") {
			B = A + _;
		} else {
			if (C) {
				B = A + "&" + _;
			} else {
				B = A + "?" + _;
			}
		}
		return B;
	};
	_.VENDOR_INFO = "channel=chunlei&clienttype=0&web=1";
	_.PCSHOST = "";
	_.MIS_EXAMINING = false;
	_.YUN = "http://yun.baidu.com";
	_.PAN = "http://pan.baidu.com";
	_.CREATE = "/api/create?a=commit&" + _.VENDOR_INFO;
	_.LIST = "/api/list?" + _.VENDOR_INFO;
	_.MIS_COUNTER = "/mis/dtcount";
	_.LIST_SHARE_HOME = "/share/homerecord?" + _.VENDOR_INFO;
	_.LIST_CATEGORY = "/api/categorylist?" + _.VENDOR_INFO;
	_.INFO_CATEGORY = "/api/categoryinfo?" + _.VENDOR_INFO;
	_.AUTO_EXPAND = "/api/autoexpandquota?" + _.VENDOR_INFO;
	_.QUOTA = "/api/quota?" + _.VENDOR_INFO;
	_.FILE_MANAGER = "/api/filemanager?" + _.VENDOR_INFO;
	_.SEARCH = "/api/search?" + _.VENDOR_INFO;
	_.OPERARECORD = "/api/operarecord?" + _.VENDOR_INFO;
	_.INVITE_GET_NUM = "/api/invite/getnum?" + _.VENDOR_INFO;
	_.ACTIVATE = "/api/invite/activate?" + _.VENDOR_INFO;
	_.INVITE_CHECK_CODE = "/api/invite/checkinvitecode?" + _.VENDOR_INFO;
	_.CMS_DATA_SIMPLE = "/disk/cmsdata";
	_.CMS_DATA = _.CMS_DATA_SIMPLE + "?" + _.VENDOR_INFO;
	_.ANALYTICS = "/api/analytics";
	_.UFO_ANALYTICS = "http://ufo.baidu.com/listen/api/addfeedback";
	_.LIST_RECYCLE = "/api/recycle/list?" + _.VENDOR_INFO;
	_.DELETE_RECYCLE = "/api/recycle/delete?" + _.VENDOR_INFO;
	_.RESTORE_RECYCLE = "/api/recycle/restore?" + _.VENDOR_INFO;
	_.CLEAR_RECYCLE = "/api/recycle/clear?" + _.VENDOR_INFO;
	_.UPDATE_WEB_VERSION = "/api/version/updatewebversion?" + _.VENDOR_INFO;
	_.UPLOAD = "https://c.pcs.baidu.com/rest/2.0/pcs/file?method=upload&type=tmpfile&app_id=250528";
	_.RATIFY_HTML5_UPLOAD = true;
	_.UPLOAD_ROUTE = "https://c.pcs.baidu.com/rest/2.0/pcs/file?method=upload&app_id=250528&ondup=newcopy&dir=%s&filename=%s";
	_.MULTI_DOWNLOAD = "https://pcs.baidu.com/rest/2.0/pcs/file?method=batchdownload&app_id=250528";
	_.SINGLE_DOWNLOAD = "https://pcs.baidu.com/rest/2.0/pcs/file?method=download&app_id=250528";
	_.THUMBNAIL_GENERATE = "https://pcs.baidu.com/rest/2.0/pcs/thumbnail?method=generate&app_id=250528";
	_.DOCPREVIEW = "https://pcs.baidu.com/rest/2.0/pcs/doc?app_id=250528";
	_.ACTIVEX_NOTIFICATION = true;
	_.ACTIVEX_USE_UPGRADE_STRATEGY = true;
	_.ACTIVEX_UPGRADE = _.CMS_DATA_SIMPLE + "?do=plugin&" + _.VENDOR_INFO;
	_.ACTIVEX_SERVER_HOST = "c.pcs.baidu.com";
	_.ACTIVEX_ORIGIN_HOST = "pan.baidu.com";
	_.UNZIP = "https://pcs.baidu.com/rest/2.0/pcs/file";
	_.PAN_URL = "http://pan.baidu.com/disk/home";
	_.CLOUD_DL = "/rest/2.0/services/cloud_dl?";
	_.TRANSFER = "/share/transfer?" + _.VENDOR_INFO;
	_.SHARE_COUNTER = "/share/autoincre?" + _.VENDOR_INFO;
	_.LINK_COUNT = "/share/totalcnt?" + _.VENDOR_INFO;
	_.LIST_SHARE = "/share/list?" + _.VENDOR_INFO;
	_.RECORD_SHARE = "/share/record?" + _.VENDOR_INFO;
	_.CANCEL_SHARE = "/share/cancel?" + _.VENDOR_INFO;
	_.SINGLE_PUBLIC_URI = "/share/singlepublicuri?" + _.VENDOR_INFO;
	_.MSGCNT_SHARE = "/share/msgcnt?" + _.VENDOR_INFO;
	_.SET_SHARE = "/share/set?" + _.VENDOR_INFO;
	_.PRISET_SHARE = "/share/priset?" + _.VENDOR_INFO;
	_.VERIFY_SHARE = "/share/verify";
	_.HOTREC_SHARE = "/share/hotrec";
	_.DOCREC_SHARE = "/share/docrec";
	_.PRODUCT_SHARE = "/pmall/product/hotproductlist";
	_.GET_UINFO = "/pcloud/user/getinfo";
	_.SET_UINFO = "/pcloud/user/editinfo";
	_.LIKE_GET = "/pcloud/like/data";
	_.LIKE_CREATE = "/pcloud/like/create";
	_.LIKE_CANCEL = "/pcloud/like/cancel";
	_.VERIFY_CAPTCHAPI = "/share/captchaip";
	_.VERIFY_VCODE = "/share/vcode";
	_.SHARE_GET_DLINK = "/share/download?" + _.VENDOR_INFO;
	_.HOME_HOT_LINKS = "/share/homehotlinks";
	_.MULTI_DOWNLOAD_PUBLIC = "/share/batchdownload?" + _.VENDOR_INFO;
	_.USER_REPORT = "/api/report/user?" + _.VENDOR_INFO;
	_.USER_INFO = "/api/account/uinfo?" + _.VENDOR_INFO;
	_.PCLOUD_ADDFOLLOW = "/pcloud/friend/addfollow?" + _.VENDOR_INFO;
	_.PCLOUD_DEFOLLOW = "/pcloud/friend/removefollow?" + _.VENDOR_INFO;
	_.TASK_CREATE = "/api/task/create";
	_.TASK_QUERY = "/api/task/query";
	_.PCS_FILE = "https://pcs.baidu.com/rest/2.0/pcs/file";
	_.SHARE_STREAM_FILE = "/share/streaming";
	_.INBOX_STREAM_AUTH = "/inbox/object/streamauth";
	_.INBOX_STREAM = "/inbox/object/streaming";
	_.SHARE_STREAM_FILE_CHECK = "/share/streamauthcheck";
	_.REVISION_LIST = "/api/revision/list";
	_.REVISION_REVERT = "/api/revision/revert";
	_.VIP_QUOTA = "/rest/2.0/membership/quota";
	_.VIP_PROPERTY = "/rest/2.0/membership/property";
	_.INBOX_FRIEND_GETFOLLOW = "/inbox/friend/getfollowlist";
	_.INBOX_FRIEND_GETFANS = "/inbox/friend/getfanslist";
	_.INBOX_FRIEND_FRIENDCOUNT = "/inbox/friend/friendcount";
	_.INBOX_FRIEND_GETFRIENDLIST = "/inbox/friend/getfriendlist";
	_.INBOX_FRIEND_RECENTFRIEND = "/inbox/friend/recentfriend";
	_.INBOX_FRIEND_QUERYFRIEND = "/inbox/friend/queryfriend";
	_.INBOX_FRIEND_QUERYUSER = "/inbox/friend/queryuser";
	_.INBOX_OBJECT_CREATE = "/inbox/object/create";
	_.INBOX_OBJECT_CHECKANTISPAM = "/inbox/object/checkantispam";
	disk.api.RestAPI = _;
})();
var Category = function () {
	this[".doc"] = Category.Word;
	this[".docx"] = Category.Word;
	this[".xls"] = Category.Excel;
	this[".xlsx"] = Category.Excel;
	this[".ppt"] = Category.Ppt;
	this[".pptx"] = Category.Ppt;
	this[".vsd"] = Category.Visio;
	this[".pdf"] = Category.Pdf;
	this[".apk"] = Category.Apk;
	this[".exe"] = Category.Exe;
	this[".msi"] = Category.Exe;
	this[".ipa"] = Category.Ipa;
	this[".zip"] = Category.Zip;
	this[".rar"] = Category.Rar;
	this[".psd"] = Category.Psd;
	this[".key"] = Category.Key;
	this[".dmg"] = Category.Dmg;
	this[".ai"] = Category.Ai;
	this[".txt"] = Category.Txt;
	this[".torrent"] = Category.Torrent;
	this[".dws"] = Category.Cad;
	this[".dwt"] = Category.Cad;
	this[".dxf"] = Category.Cad;
	this[".dwg"] = Category.Cad;
	this[".mmap"] = Category.Mmap;
	this[".xmind"] = Category.Xmind;
	this[".mm"] = Category.Mm;
};
Category.HASH = [0, 2, 3, 1, 4, 5, 6, 7];
Category.MUS = 2;
Category.PIC = 3;
Category.MOV = 1;
Category.DOC = 4;
Category.Doc_oth = 4;
Category.APP = 5;
Category.OTH = 6;
Category.Txt = 7;
Category.Excel = 40;
Category.Word = 41;
Category.Ppt = 42;
Category.Visio = 43;
Category.Pdf = 44;
Category.Apk = 51;
Category.Exe = 50;
Category.Ipa = 52;
Category.Zip = 63;
Category.Torrent = 62;
Category.Rar = 61;
Category.Psd = 65;
Category.Key = 66;
Category.Dmg = 67;
Category.Ai = 68;
Category.BT = 7;
Category.Cad = 70;
Category[Category.Doc_oth] = function (_) {
	FileUtils._mDiskLog.send({
		type : "docopen"
	});
	if (FileUtils._mDocumentReader == null) {
		Utilities.useToast({
			toastMode : disk.ui.Toast.MODE_LOADING,
			msg : "\u6b63\u5728\u52a0\u8f7d\u6587\u6863\u9605\u8bfb\u7ec4\u4ef6,\u8bf7\u7a0d\u5019\u2026\u2026",
			sticky : false
		});
		ark.use({
			url : ["/buss/dialog/docreader_dialog.js", "/dialog/doc-dialog.css"],
			noDependency : true
		}, function () {
			Utilities.useCloseToast();
			if (typeof disk.ui.DocReader != "undefined") {
				FileUtils._mDocumentReader = new disk.ui.DocReader(_);
			} else {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u6587\u6863\u9605\u8bfb\u7ec4\u4ef6\u52a0\u8f7d\u5931\u8d25,\u8bf7\u7a0d\u5019\u91cd\u8bd5",
					sticky : false
				});
			}
			FileUtils._mDocumentReader.reBuild(_);
		});
	} else {
		FileUtils._mDocumentReader.reBuild(_);
	}
};
Category[Category.MOV] = function (B) {
	if (disk.DEBUG) {
		console.log(B);
	}
	if (!(Page.inViewMode(Page.VIEW_HOME) || Page.inViewMode(Page.VIEW_PROPERTY_OTHER))) {
		return;
	}
	var C;
	if (Page.inViewMode(Page.VIEW_HOME)) {
		C = "/play/video#video/path=" + encodeURIComponent(B.path) + "&t=-1";
		var A = disk.util.HistoryManager.getDefault().getCurrentHistoryState(),
		_ = disk.util.HistoryManager.parseHistory(A);
		if (_ && _.length > 0 && _[0] == "category") {
			C += "&cate=1";
		}
	} else {
		if (FileUtils && B && typeof FileUtils.share_uk && FileUtils.share_id && B.fs_id) {
			C = "http://" + location.host + "/share/link?shareid=" + encodeURIComponent(FileUtils.share_id) + "&uk=" + encodeURIComponent(FileUtils.share_uk) + "&fid=" + encodeURIComponent(B.fs_id);
		}
	}
	if (C) {
		window.open(C);
	}
};
Category[Category.PIC] = function (A, E, B) {
	var B = B,
	_ = $(".loadedDate span").html(),
	D = FileUtils.getDirMgr().getDir() ? FileUtils.getDirMgr().getDir() : "/",
	C = function () {
		if (FileUtils._mImgPreviewer == null) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_LOADING,
				msg : "\u6b63\u5728\u52a0\u8f7d\u56fe\u7247\u9884\u89c8\u7ec4\u4ef6,\u8bf7\u7a0d\u5019\u2026\u2026",
				sticky : false
			});
			ark.use({
				url : ["/buss/dialog/imgpreview_dialog.js", "/dialog/img-view-dialog.css"],
				noDependency : false
			}, function () {
				Utilities.useCloseToast();
				if (typeof disk.ui.ImgPreviewer != "undefined") {
					FileUtils._mImgPreviewer = new disk.ui.ImgPreviewer(A);
				} else {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u56fe\u7247\u9884\u89c8\u7ec4\u4ef6\u52a0\u8f7d\u5931\u8d25,\u8bf7\u7a0d\u5019\u91cd\u8bd5",
						sticky : false
					});
				}
				FileUtils._mImgPreviewer.reBuild(A);
			});
		} else {
			setTimeout(function () {
				FileUtils._mImgPreviewer.reBuild(A);
			}, 200);
		}
	};
	C();
};
Category[Category.APP] = function (_) {
	if (disk.DEBUG) {
		console.log(_);
	}
};
Category[Category.OTH] = function (_, C) {
	if (disk.DEBUG) {
		console.log(_);
	}
	var A = (_.path && _.path.slice(_.path.lastIndexOf("."))) || "";
	A = A.toLowerCase();
	if (!FileUtils.acceptOpenFileTypes(A)) {
		return;
	}
	if (A == ".torrent") {
		if (disk.DEBUG) {
			console.log("open torrent file : " + _.path);
		}
		if (FileUtils._mResolveBTOffline == null) {
			FileUtils._mResolveBTOffline = disk.ui.ResolveBTDialog.obtain();
		}
		if (typeof FileUtils._mResolveBTOffline.setTaskFrom == "function") {
			FileUtils._mResolveBTOffline.setTaskFrom(disk.ui.ResolveBTDialog.TASK_FROM_HOMEPAGE);
		}
		FileUtils._mResolveBTOffline.resolveBTFile(_.path);
	} else {
		if (A == ".zip" || A == ".rar") {
			if (disk.DEBUG) {
				console.log("open ", A, " file : " + _.path);
			}
			$("#wizard-functions-intro").remove();
			var B = function () {
				FileUtils._mResolveUNZip.startResolveUnZip(_.path, _.size);
			},
			D = function () {
				var _ = Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u7f51\u7edc\u5f02\u5e38\uff0c\u8bf7\u7a0d\u5019\u518d\u8bd5",
						closeType : true,
						sticky : true
					});
				_.setVisible(true, true);
				_.setGravity(disk.ui.Panel.TOP);
			};
			if (FileUtils._mResolveUNZip == null) {
				if (typeof disk.ui.UnZipDialog == "undefined") {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_LOADING,
						msg : "\u6b63\u5728\u6253\u5f00\u6587\u4ef6\u9884\u89c8\u5217\u8868\uff0c\u8bf7\u7a0d\u5019&hellip;",
						sticky : true
					}).setVisible(true, true);
					$.ajax({
						url : "/res/static/js/buss/dialog/unzip_dialog.js?t=201404241528",
						dataType : "script",
						success : function () {
							if (typeof disk.ui.UnZipDialog == "function") {
								FileUtils._mResolveUNZip = disk.ui.UnZipDialog.obtain();
								B();
							} else {
								if (disk.DEBUG) {
									console.warn("loaded constructor UnZipDialog failed");
								}
								D();
							}
						}
					}).error(function () {
						D();
					});
				} else {
					FileUtils._mResolveUNZip = disk.ui.UnZipDialog.obtain();
					B();
				}
			} else {
				B();
			}
		} else {
			if (A == ".html") {
				if (_.path) {
					window.open("http://wenzhang.baidu.com/pcs/openfile?fs_id=" + encodeURIComponent(_.fs_id));
				}
			} else {
				if (A == ".dws" || A == ".dwt" || A == ".dxf" || A == ".dwg") {
					if (_.path) {
						window.open("/disk/cadedit?path=" + encodeURIComponent(encodeURIComponent(_.path)));
					}
				}
			}
		}
	}
};
Category[Category.BT] = function (_, A) {
	if (!Page.inViewMode(Page.VIEW_HOME)) {
		return;
	}
	if (FileUtils._mResolveBTOffline == null) {
		FileUtils._mResolveBTOffline = disk.ui.ResolveBTDialog.obtain();
	}
	if (typeof FileUtils._mResolveBTOffline.setTaskFrom == "function") {
		FileUtils._mResolveBTOffline.setTaskFrom(disk.ui.ResolveBTDialog.TASK_FROM_HOMEPAGE);
	}
	FileUtils._mResolveBTOffline.resolveBTFile(_.path);
};
Category[Category.MUS] = function (_, B) {
	var A = [{
			songId : FileUtils._mMboxPlayerIndex++,
			songName : FileUtils.parseDirFromPath(_.path)
		}
	],
	D = function (_) {
		A[0].link = _;
		if (FileUtils._mMboxPlayer == null) {
			FileUtils._mMboxPlayer = new disk.ui.MusicBox(A);
		} else {
			FileUtils._mMboxPlayer.reBuild(A, B.button);
		}
	};
	if (Page.inViewMode(Page.VIEW_HOME)) {
		D(Page.obtainDownloadURL(_) || _.musicpreview);
	} else {
		var C = disk.Context.getService(disk.Context.SERVICE_DOWNLOAD_PROXY);
		C._checkDownloadFile({
			fs_id : _.fs_id
		}, 1, "", "", true, function (_) {
			if (_) {
				D(_);
			}
		});
	}
};
Category.typeMap = Category.prototype = {
	".wma" : Category.MUS,
	".wav" : Category.MUS,
	".mp3" : Category.MUS,
	".aac" : Category.MUS,
	".ra" : Category.MUS,
	".ram" : Category.MUS,
	".mp2" : Category.MUS,
	".ogg" : Category.MUS,
	".aif" : Category.MUS,
	".mpega" : Category.MUS,
	".amr" : Category.MUS,
	".mid" : Category.MUS,
	".midi" : Category.MUS,
	".jpg" : Category.PIC,
	".jpeg" : Category.PIC,
	".gif" : Category.PIC,
	".bmp" : Category.PIC,
	".png" : Category.PIC,
	".jpe" : Category.PIC,
	".cur" : Category.PIC,
	".svg" : Category.PIC,
	".svgz" : Category.PIC,
	".tif" : Category.PIC,
	".tiff" : Category.PIC,
	".ico" : Category.PIC,
	".wmv" : Category.MOV,
	".rmvb" : Category.MOV,
	".mpeg4" : Category.MOV,
	".mpeg2" : Category.MOV,
	".flv" : Category.MOV,
	".avi" : Category.MOV,
	".3gp" : Category.MOV,
	".mpga" : Category.MOV,
	".qt" : Category.MOV,
	".rm" : Category.MOV,
	".wmz" : Category.MOV,
	".wmd" : Category.MOV,
	".wvx" : Category.MOV,
	".wmx" : Category.MOV,
	".wm" : Category.MOV,
	".swf" : Category.MOV,
	".mpg" : Category.MOV,
	".mp4" : Category.MOV,
	".mkv" : Category.MOV,
	".ts" : Category.MOV,
	".asf" : Category.MOV,
	".mpeg" : Category.MOV,
	".mov" : Category.MOV,
	".doc" : Category.DOC,
	".docx" : Category.DOC,
	".xls" : Category.DOC,
	".xlsx" : Category.DOC,
	".ppt" : Category.DOC,
	".pptx" : Category.DOC,
	".vsd" : Category.DOC,
	".txt" : Category.DOC,
	".pdf" : Category.DOC,
	".key" : Category.DOC,
	".ods" : Category.DOC,
	".ots" : Category.DOC,
	".odt" : Category.DOC,
	".rtf" : Category.DOC,
	".dot" : Category.DOC,
	".dotx" : Category.DOC,
	".odm" : Category.DOC,
	".pps" : Category.DOC,
	".pot" : Category.DOC,
	".xlt" : Category.DOC,
	".xltx" : Category.DOC,
	".csv" : Category.DOC,
	".ppsx" : Category.DOC,
	".potx" : Category.DOC,
	".apk" : Category.APP,
	".exe" : Category.APP,
	".msi" : Category.APP,
	".ipa" : Category.APP,
	".dwg" : Category.CAD,
	".dwt" : Category.CAD,
	".dxf" : Category.CAD,
	".dws" : Category.CAD,
	".mmap" : Category.MMAP,
	".xmind" : Category.XMIND,
	".mm" : Category.MM
};
Category.iconMap_X = {
	6 : -416,
	40 : -128,
	41 : -96,
	42 : -256,
	43 : -32,
	44 : -64,
	4 : -96,
	50 : -320,
	51 : -384,
	52 : -288,
	1 : -224,
	2 : -192,
	3 : 0,
	61 : -352,
	63 : -352,
	7 : -160,
	62 : -448,
	65 : -512,
	66 : -545,
	67 : -320,
	68 : -578,
	70 : 0,
	71 : -32,
	72 : -64,
	73 : -96
};
Category.singleton = (function () {
	if (Category.iconMap == null) {
		Category.iconMap = new Category();
	}
	return Category.iconMap;
})();
var unify = {
	st : {},
	app : {},
	getLogger : function () {
		return unify.st.SystemLog.obtain();
	},
	DEBUG : false
};
(function (_) {
	_.isFunction = function (_) {
		return (typeof _ === "function") || Object.prototype.toString.apply(_) === "[object Function]";
	};
	_.hasOwnProperty = (Object.prototype.hasOwnProperty) ? function (_, A) {
		return _ && _.hasOwnProperty && _.hasOwnProperty(A);
	}
	 : function (_, A) {
		return (typeof _[A] != "undefined") && _.constructor.prototype[A] !== _[A];
	};
	_._IEEnumFix = (typeof window.attachEvent != "undefined") ? function (C, F) {
		var A,
		B,
		D,
		E = ["toString", "valueOf"];
		for (A = 0; A < E.length; A = A + 1) {
			B = E[A];
			D = F[B];
			if (_.isFunction(D) && D != Object.prototype[B]) {
				C[B] = D;
			}
		}
	}
	 : function () {};
	_.extend = function (E, D, B) {
		if (!D || !E) {
			throw new Error("extend failed, please check that " + "all dependencies are included.");
		}
		var C = function () {},
		A;
		C.prototype = D.prototype;
		E.prototype = new C();
		E.prototype.constructor = E;
		E.superclass = D.prototype;
		if (D.prototype.constructor == Object.prototype.constructor) {
			D.prototype.constructor = D;
		}
		if (B) {
			for (A in B) {
				if (_.hasOwnProperty(B, A)) {
					E.prototype[A] = B[A];
				}
			}
			_._IEEnumFix(E.prototype, B);
		}
	};
	var B = function () {
		this._mCallbackListener = null;
		this._mCallbackContext = null;
		this._mCallbackBaseDir = null;
		this._mCacheMap = {};
		this._mQulifierCache = {};
		this._mSessionTask = null;
	};
	B._sCounter = 0;
	B.getQulifier = function () {
		return ++B._sCounter;
	};
	B.JOIN_TOKEN = "__";
	B.PATTERN_DELIMTER = /\//gi;
	B.ALPHABETICAL_SORTOR = function (_, A) {
		return _.localeCompare(A);
	};
	B.prototype = {
		findClass : function (I, H) {
			if (typeof I != "number") {
				throw new Error("you should specify an " + "idenfier to initial an request");
			}
			if (H == null || H.length == 0) {
				throw new Error("you need to specify " + "at least one class to load");
			}
			if (typeof this._mQulifierCache[I] != "undefined") {
				this._callback0(I);
				return;
			}
			var E = [],
			F = null,
			D = [];
			for (var C = 0, A = H.length; C < A; C++) {
				var G = H[C];
				D.push(G);
				E.push(this._toSuffix(G));
				if (C == 0) {
					F = this._toPrefix(G);
				}
			}
			var _ = F + "/" + E.join(B.JOIN_TOKEN);
			if (this._cached(_)) {
				this._callback0(I);
				return;
			}
			this._mSessionTask = D;
			this._start(_, I);
		},
		_start : function (_, B) {
			if (this._mSessionTask == null || this._mSessionTask.length == 0) {
				this._callback0(B);
				this._mCacheMap[this.toHashKey(_)] = true;
				this._mSessionTask = null;
				return;
			}
			var A = this._mSessionTask.shift();
			if (this._cached(A)) {
				this._start(_, B);
			} else {
				this._callback1(_, B, A);
			}
		},
		setCallback : function (_, A, B) {
			this._mCallbackListener = _;
			this._mCallbackContext = A;
			this._mCallbackBaseDir = B;
		},
		_callback1 : function (_, D, A) {
			var C = this;
			if (this._mCallbackListener != null && this._mCallbackContext != null) {
				var B = this._mCallbackBaseDir + A + ".js";
				this._mCallbackListener.onClassLoaderLoadFile.call(this._mCallbackContext, D, B, function () {
					var B = C.toHashKey(A);
					C._mCacheMap[B] = true;
					C._start(_, D);
				}, function () {
					C._callback2(D);
				});
			} else {
				this._callback2(D);
			}
		},
		_callback2 : function (_) {
			if (this._mCallbackListener != null && this._mCallbackContext != null) {
				this._mCallbackListener.onClassLoaderLoadFailure.call(this._mCallbackContext, _);
			}
			this._mSessionTask = null;
		},
		_callback0 : function (_) {
			if (this._mCallbackListener != null && this._mCallbackContext != null) {
				this._mCallbackListener.onClassLoaderLoadComplete.call(this._mCallbackContext, _);
			}
		},
		_cached : function (_) {
			return typeof this._mCacheMap[this.toHashKey(_)] != "undefined";
		},
		_toSuffix : function (_) {
			return _.substring(_.lastIndexOf("/") + 1);
		},
		_toPrefix : function (_) {
			return _.substring(0, _.lastIndexOf("/"));
		},
		toHashKey : function (_) {
			return _.replace(B.PATTERN_DELIMTER, "_");
		}
	};
	_.st.ClassLoader = B;
	var C = function () {
		this._mPrivateFlags = 0;
		this._mResizeTimer = null;
		this._mRMode = -1;
		this._mLastRMode = -1;
		this._mMsgLooperHandler = null;
		this._mBootArgs = null;
		this._mCrossScreen = true;
		this._mVisualRootDirOfJS = null;
		this._mClassLoader = null;
		this._mFeatureCache = {};
		this._mAsyncCallContext = {};
	};
	C.LR = 0;
	C.MR = 1;
	C.HR = 2;
	C.DELAY = 50;
	C.DEFAULT_LOOP_PERIOD = 3000;
	C.STARTED = 1;
	C.MESSAGE_LOOPER = 2;
	C.INSTALL_LOGGING_POSTER = 4;
	C.CLASS_LOADER_OVERRIDE = 8;
	C.ID_UNIFY_RUNTIME_FLAG = "unify-runtime-flag-as-id";
	C.prototype = {
		main : function (_) {
			this._mBootArgs = _;
			var B = this;
			if (_ != null) {
				if (typeof _[0] == "string" && _[0].length > 0) {
					this._mVisualRootDirOfJS = _[0];
				}
				if (typeof _[1] == "boolean") {
					this._mCrossScreen = _[1];
				}
			}
			this._installClassLoader();
			$(window).bind("resize", function () {
				B._onSize0();
			});
			if (typeof window.onorientationchange != "undefined") {
				window.onorientationchange = function () {
					B.onOrientationChange(window.orientation);
				};
			}
			window.onbeforeunload = function () {
				var _ = B.dispatchOnStop();
				if (typeof _ === "string" && _.trim().length > 0) {
					return _;
				}
			};
			var A = C.STARTED;
			this._mPrivateFlags &= ~A;
			this.onStart(_);
			if ((this._mPrivateFlags & A) != A) {
				throw new Error("you need to override " + "onStart abstract method to indicating page was started");
			}
			if (this._mCrossScreen) {
				this._resolveRMode();
				switch (this._mRMode) {
				case C.LR:
					this.onLRStart(_);
					this.onLRResume(_);
					break;
				case C.MR:
					this.onMRStart(_);
					this.onMRResume(_);
					break;
				case C.HR:
					this.onHRStart(_);
					this.onHRResume(_);
					break;
				}
				this.onPostStart(_);
			}
		},
		getArgumentAt : function (_) {
			if (this._mBootArgs == null || typeof this._mBootArgs[_] == "undefined") {
				return null;
			}
			return this._mBootArgs[_];
		},
		getClassLoader : function () {
			return this._mClassLoader;
		},
		_installClassLoader : function () {
			this._mClassLoader = new _.st.ClassLoader();
			this._mClassLoader.setCallback(this, this, this._mVisualRootDirOfJS);
		},
		onClassLoaderLoadFile : function (E, B, A, D) {
			var _ = C.CLASS_LOADER_OVERRIDE;
			this._mPrivateFlags &= ~_;
			this.onFeatureRequest(E, B, A, D);
			if ((this._mPrivateFlags & _) != _) {
				throw new Error("you need override onFeatureRequest to " + "handler feature requlites files loading");
			}
		},
		onClassLoaderLoadComplete : function (_) {
			this._mFeatureCache[_] = true;
			this.onFeatureLoading(_, false);
			this.dispatchFeatureReady(_, true);
		},
		asyncCall : function (A, _, B) {
			this._mAsyncCallContext[A] = B;
			this.requestFeature(A, _);
		},
		proceedAsyncCall : function (_) {
			var A = this._mAsyncCallContext[_];
			if (typeof A != "undefined") {
				A.call(A);
				delete this._mAsyncCallContext[_];
				return true;
			} else {
				return false;
			}
		},
		onFeatureRequest : function (C, A, _, B) {},
		dispatchFeatureReady : function (A, _) {
			if (_ === true && this.proceedAsyncCall(A)) {
				return;
			}
			this.onFeatureReady(A, _);
		},
		onFeatureReady : function (A, _) {},
		onFeatureLoading : function (A, _) {},
		onClassLoaderLoadFailure : function (_) {
			this.onFeatureLoading(_, false);
			this.dispatchFeatureReady(_, false);
		},
		requestFeature : function (A, _) {
			this.onFeatureLoading(A, true);
			if (typeof this._mFeatureCache[A] != "undefined") {
				this.onClassLoaderLoadComplete(A);
			} else {
				this._mClassLoader.findClass(A, _);
			}
		},
		log : function (B, A) {
			if (_.DEBUG && typeof console.log == "function") {
				console.log("[" + B + "]" + A);
			}
		},
		assert : function (C, A, B) {
			if (_.DEBUG && typeof console.assert == "function") {
				console.assert(A, C + B);
			}
		},
		findViewById : (typeof document.querySelector != "undefined") ? function (_) {
			return document.querySelector("#" + _);
		}
		 : function (_) {
			return document.getElementById(_);
		},
		setMessageLoopEnabled : function (D, _) {
			var A = C.MESSAGE_LOOPER,
			B = this;
			if (typeof _ == "undefined") {
				_ = C.DEFAULT_LOOP_PERIOD;
			}
			if (D) {
				this._mPrivateFlags |= A;
				if (this._mMsgLooperHandler != null) {
					clearInterval(this._mMsgLooperHandler);
				}
				this._mMsgLooperHandler = setInterval(function () {
						B.dispatchMessageLoop(_);
					}, _);
			} else {
				this._mPrivateFlags &= ~A;
				clearInterval(this._mMsgLooperHandler);
				this._mMsgLooperHandler = null;
			}
		},
		isMessageLoopEnabled : function () {
			var _ = C.MESSAGE_LOOPER;
			return (this._mPrivateFlags & _) == _;
		},
		_resolveRMode : function () {
			var A = document.getElementById(C.ID_UNIFY_RUNTIME_FLAG);
			if (!A) {
				throw new Error("runtime window flag not found, pls check your web page");
			}
			var _ = 0;
			if (typeof window.getComputedStyle != "undefined") {
				_ = window.getComputedStyle(A, null).left;
			} else {
				_ = A.currentStyle.left;
			}
			_ = parseInt(_);
			if (isNaN(_)) {
				throw new Error("runtime window flag was malformed, pls check your web page");
			}
			switch (_) {
			case  - 20:
				this._mRMode = C.MR;
				break;
			case  - 30:
				this._mRMode = C.LR;
				break;
			default:
				this._mRMode = C.HR;
				break;
			}
		},
		getRMode : function () {
			if (this._mRMode == -1) {
				throw new Error("RMode has not resolved yet, " + "maybe runtime error encoutered");
			}
			return this._mRMode;
		},
		getDisplayPixelRatio : function () {
			if (typeof window.devicePixelRatio == "undefined") {
				return 1;
			}
			return window.devicePixelRatio;
		},
		dispatchOnSize : function () {
			if (this._mCrossScreen) {
				var A = this._mRMode;
				this._resolveRMode();
				var _ = this._mRMode;
				if (_ != A) {
					if (A != -1) {
						switch (A) {
						case C.HR:
							this.onHRPause(this._mBootArgs);
							break;
						case C.MR:
							this.onMRPause(this._mBootArgs);
							break;
						case C.LR:
							this.onLRPause(this._mBootArgs);
							break;
						}
					}
					switch (_) {
					case C.HR:
						this.onHRResume(this._mBootArgs);
						break;
					case C.MR:
						this.onMRResume(this._mBootArgs);
						break;
					case C.LR:
						this.onLRResume(this._mBootArgs);
						break;
					}
				}
				this._mLastRMode = A;
			}
			this.onSize();
		},
		_onSize0 : function () {
			if (this._mResizeTimer != null) {
				clearTimeout(this._mResizeTimer);
			}
			var _ = this;
			this._mResizeTimer = setTimeout(function () {
					_._mResizeTimer = null;
					_.dispatchOnSize();
				}, C.DELAY);
		},
		dispatchMessageLoop : function (_) {
			this.onMessageLoop(_);
		},
		onMessageLoop : function (_) {},
		dispatchOnStop : function () {
			return this.onStop();
		},
		onStop : function () {},
		onStart : function (_) {},
		onPostStart : function (_) {},
		onLRStart : function (_) {},
		onLRPause : function (_) {},
		onLRResume : function (_) {},
		onMRStart : function (_) {},
		onMRPause : function (_) {},
		onMRResume : function (_) {},
		onHRStart : function (_) {},
		onHRPause : function (_) {},
		onHRResume : function (_) {},
		onSize : function () {},
		onOrientationChange : function (_) {},
		dispatchWindowStarted : function () {
			this._mPrivateFlags |= C.STARTED;
		},
		logging : function (A, _) {},
		onSendLog : function (_) {},
		setBooleanFlag : function (A, _) {
			if (_) {
				this._mPrivateFlags |= A;
			} else {
				this._mPrivateFlags &= ~A;
			}
		}
	};
	_.st.AbstractPageWindow = C;
	var A = function () {
		this._mRootClass = null;
		this._mPrivateFlags = 0;
		this._mAbstractPageWindow = null;
	};
	A.main = function (B, _) {
		A.getInstance().setRootClass(B);
		A.getInstance().bootup(_);
	};
	A.getPageWindow = function () {
		return A.getInstance().getPageWindow();
	};
	A._sInstance = null;
	A.getInstance = function () {
		if (A._sInstance == null) {
			A._sInstance = new A();
		}
		return A._sInstance;
	};
	A.BOOT = 1;
	A.prototype = {
		setRootClass : function (_) {
			this._mRootClass = _;
		},
		getPageWindow : function () {
			return this._mAbstractPageWindow;
		},
		bootup : function (B) {
			var C = A.BOOT;
			if ((this._mPrivateFlags & C) != C) {
				var D = this._mRootClass;
				if (D == null) {
					throw new Error("no boot class found, unify runtime is shuting down");
				}
				this._mAbstractPageWindow = new D();
				if (!(this._mAbstractPageWindow instanceof _.st.AbstractPageWindow)) {
					throw new Error("boot class must be instance of unify.st.AbstractPageWindow, unify runtime is shuting down");
				}
				this._mAbstractPageWindow.main(B);
				this._mPrivateFlags |= C;
			}
		}
	};
	_.st.Bootstrap = A;
})(unify);
var Utilities = {
	_mSortingColsHandler : null,
	_mSortingColsWakedup : null,
	_mTotalSpacing : null,
	_mUsedSpacing : null,
	_mInterval : 6 * 60 * 60 * 1000,
	keyCode : function (A, _) {
		A.keydown(function (B) {
			var A = B ? B : window.event;
			switch (A.keyCode) {
			case 27:
				_[27]();
				break;
			case 40:
				_[40]();
				break;
			case 39:
				_[39]();
				break;
			case 38:
				_[38]();
				break;
			case 37:
				_[37]();
				break;
			case 34:
				_[34]();
				break;
			case 33:
				_[33]();
				break;
			case 13:
				_[13]();
				break;
			}
		});
	},
	mouseWheel : function (_, C, B) {
		var A = function (A) {
			var A = A || window.event || arguments[0],
			_ = 0;
			if (A.wheelDelta) {
				_ = -A.wheelDelta / 120;
			} else {
				if (A.detail) {
					_ = A.detail / 3;
				}
			}
			if (_) {
				if ("string" == typeof B || B instanceof String) {
					C(_ > 0 ? true : false);
				} else {
					if ("number" == typeof B || B instanceof Number) {
						C(_ * B);
					}
				}
			}
			if (A.preventDefault) {
				A.preventDefault();
			} else {
				A.returnValue = false;
			}
		};
		if (typeof window.attachEvent != "undefined") {
			_.attachEvent("onmousewheel", A);
		} else {
			if ("onmousewheel" in window) {
				_.onmousewheel = A;
			} else {
				if (typeof window.addEventListener != "undefined") {
					_.addEventListener("DOMMouseScroll", A, false);
				}
			}
		}
	},
	resolveLayout : function (E) {
		var _ = $("#fileList"),
		F = $("#subwindowContainer").offset().top,
		B,
		C = document.body.className,
		D = $(window).height(),
		G = $(window).width();
		_.length && (B = _.offset().top);
		var A = 300;
		if (G < 1024) {
			$(".aside").css("width", "160px");
			$(".b-uploader", ".aside").css("left", "7px");
			$(".b-list-3 a .text1").css("margin-left", "26px");
			if (!Page.inViewMode(Page.VIEW_PROPERTY_OTHER) && !Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				$("#subwindowContainer").css("margin-left", "160px");
			}
			$("#upload-dialog-btn").css("padding-left", "5px");
			$("#scrollbar,#scrollbarGrid").css({
				left : $(".doc:first").width() - 16,
				right : "auto"
			});
			if (document.getElementsByTagName("iframe")) {
				if ($("iframe").css("margin-left") == 0) {
					$("iframe").css("margin-left", "0px");
				}
			}
			$("#awardBubbleP").css("margin-left", "12px");
			$(".side-options").css("margin-left", "2px");
			$(".share-warn-tit").css("marginLeft", "14px");
		} else {
			$(".aside").css("width", "210px");
			$(".b-uploader", ".aside").css("left", "26px");
			$("#upload-dialog-btn").css("padding-left", "25px");
			if (!Page.inViewMode(Page.VIEW_PROPERTY_OTHER) && !Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				$("#subwindowContainer").css("margin-left", "209px");
			}
			$("#scrollbar,#scrollbarGrid").css({
				left : "auto",
				right : "0px"
			});
			if (document.getElementsByTagName("iframe")) {
				$("iframe").css("margin-left", "0px");
			}
			$("#awardBubbleP").css("margin-left", "28px");
			if ($(".award-img-section")) {
				$(".award-img-section").css("margin-left", "15px");
			}
			$(".side-options").css("margin-left", "18px");
			$(".share-warn-tit").css("marginLeft", "44px");
		}
		asideMenuOffsetTop = F + ($("#upload-dialog-btn").height() ? $("#upload-dialog-btn").height() : 0);
		sideOptions = $(".side-options").height() + 15;
		if (D > A) {
			if ($("#awardBubbleP:visible").length) {
				$("#genere").height(D - asideMenuOffsetTop - 10);
			} else {
				$("#genere").height(D - asideMenuOffsetTop);
			}
			if (E) {
				$("#subwindowContainer").height(D - F - 3);
				return;
			}
			_.height(D - B);
			$(".flag2-pan-right").height($("#subwindowContainer").height());
			$(".flag10-pan-right").height($("#subwindowContainer").height() - $(".homeheader").height());
		} else {
			$("#genere").height(D - asideMenuOffsetTop - 10);
			if (E) {
				$("#subwindowContainer").height(A - F - 3);
				return;
			}
			_.height(A - B);
		}
		var K = 26,
		H = 0;
		if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
			if ($(".flag2-fns").length > 0) {
				H = $(".flag2-fns").offset().top;
			}
		} else {
			if ($(".all-files-headers").length > 0) {
				H = $(".all-files-headers").offset().top;
			}
		}
		if (Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
			$("#scrollbar").css("left", $("#sortColsHeader").offset().left + $("#sortColsHeader").width());
		}
		$("#scrollbar,#scrollbarGrid").css("top", H);
		$("#scrollbarTracker,#scrollbarTrackerGrid").css("height", D - H - K);
		if (disk.DEBUG) {
			console.log("reset scrollBar css");
		}
		if ($.browser.msie && $.browser.version < 7) {
			$("#scrollbarArrowDown").css("position", "static").css("position", "absolute");
		}
		if ($(".no-result-file-download").length) {
			var I = $(".no-result-file-download"),
			J = _.height() - 30;
			I.height(J);
			if ($.browser.msie && $.browser.version == "6.0") {
				if (I.width() < 855) {
					$(".no-result-file-bottom").width("auto");
				} else {
					$(".no-result-file-bottom").width(788);
				}
			}
		}
		if (Page.inViewMode(Page.VIEW_HOME) && FileUtils.getModule() === "category" && FileUtils.getType() === 2) {
			var L = $("#timeLineGuide");
			if (L.length > 0) {
				L.css("top", $(".all-files-headers").offset().top - L.height());
			}
		}
	},
	layoutSubwindow : function (A) {
		var _ = document.getElementById("subwindowContainer");
		A.style.width = $(_).width() + "px";
		A.style.height = $(_).height() + "px";
		A.style.top = "0";
		A.style.left = "0";
		A.style.position = "absolute";
		A.style.background = "#fff";
		_.appendChild(A);
	},
	updateUsageQuota : function (D, E, H, C) {
		var F = document.getElementById("remainingSpaceUi_tail"),
		G = document.getElementById("remainingSpace"),
		A,
		_,
		B;
		A = Math.floor(Math.min(100, D) * 155 / 100);
		if (F) {
			F.style.width = A + "px";
		}
		if (G && (typeof E != "undefined")) {
			G.innerHTML = FileUtils.toFriendlyFileSize(H);
		}
		$(F).attr("title") && $(F).removeAttr("title");
		$("#awardBubbleP ul li").first().addClass("hidden");
		if (FileUtils.quota.expire == "true") {
			$("#awardBubbleP ul li").first().removeClass("hidden");
		}
		if (D >= 90 && E >= 0) {
			$(F).css("background", "#FC7736");
			$(F).attr("title", "\u4f60\u4f7f\u7528\u7684\u5bb9\u91cf\u5df2\u8d85\u8fc790%\u5566");
		} else {
			if (E < 0) {
				$(F).css("background", "#c00");
				$("#awardBubbleP ul li").first().removeClass("hidden");
				$("#awardBubbleP span.pay-quota").html("\u4f60\u7684\u5bb9\u91cf\u5230\u671f\u4e86");
				$("#awardBubbleP a.pay-quota").html("\u8d2d\u4e70").attr("href", "http://yun.baidu.com/buy/center?tag=2");
			} else {
				$(F).css("background", "#3482DA");
			}
		}
		if (typeof C != "undefinded" && C) {
			$("#remainingSpace").siblings(":first").text("/" + FileUtils.toFriendlyFileSize(C));
		}
	},
	updateSpacing : function (_, A) {
		this._mTotalSpacing = parseInt(_);
		this._mUsedSpacing = parseInt(A);
	},
	hasEnoughSpacing : function (A) {
		var _ = (typeof A == "number") ? A : 0;
		return _ + this._mUsedSpacing < this._mTotalSpacing;
	},
	isOverdue : function () {
		var F = this;
		if ($("#remainingSpaceUi_tail").length == 0) {
			return;
		}
		var _ = FileUtils.quota,
		D = parseFloat(_.total) - parseFloat(_.used);
		F.updateSpacing(_.total, _.used);
		var B = parseFloat((_.used * 100 / _.total).toFixed(4)),
		C = parseFloat(_.total - _.used),
		E = parseFloat(_.used),
		A = parseFloat(_.total);
		Utilities.updateUsageQuota(B, C, E, A);
		if (_.expire == "true") {
			$("#awardBubbleP ul li").first().removeClass("hidden");
		}
	},
	isFullAward : function () {
		var _ = 15 * 1024 * 1024 * 1024;
		if (FileUtils.quota.free == "null" || parseFloat(FileUtils.quota.free) < _) {
			$(".award-img-section").show();
			$("#awardBubbleP .b-no-ln").html("\u514d\u8d39\u6269\u5bb9").attr("href", "http://yun.baidu.com/disk/award");
		}
	},
	updateQuotaExpireData : function (_) {
		if (!_ || !FileUtils || !FileUtils.quota) {
			return;
		}
		if (typeof _.total != "undefined") {
			FileUtils.quota.total = _.total;
		}
		if (typeof _.used != "undefined") {
			FileUtils.quota.used = _.used;
		}
		if (typeof _.expire != "undefined") {
			FileUtils.quota.expire = "" + _.expire;
		}
		if (typeof _.free != "undefined") {
			FileUtils.quota.free = _.free;
		}
	},
	scheduleQueryQuota : function (A, _, B) {
		var C = this;
		param = {};
		if (_) {
			param.checkexpire = 1;
		}
		if (B) {
			param.checkfree = 1;
		}
		$.get(disk.api.RestAPI.QUOTA + "&t=" + (new Date().getTime()), param, function (E) {
			if (disk.DEBUG) {
				console.log("[LOG]got quota detail:" + E);
			}
			var _ = null;
			try {
				_ = $.parseJSON(E);
			} catch (G) {}

			if (_ != null && _.errno == 0) {
				C.updateSpacing(_.total, _.used);
				var D = (_.used * 100 / _.total).toFixed(4),
				F = _.total - _.used,
				H = _.used,
				B = _.total;
				Utilities.updateQuotaExpireData(_);
				Utilities.updateUsageQuota(D, F, H, B);
				if (typeof A === "function") {
					A(B, H);
				}
			}
		});
	},
	toDateString : function (C) {
		if (!this._dateFormater) {
			this._dateFormater = new Date();
		}
		this._dateFormater.setTime(C * 1000);
		var _ = this._dateFormater.getMonth() + 1,
		D = this._dateFormater.getDate(),
		A = this._dateFormater.getHours(),
		B = this._dateFormater.getMinutes();
		return this._dateFormater.getFullYear() + "-" + (String(_).length < 2 ? "0" + _ : _) + "-" + (String(D).length < 2 ? "0" + D : D) + " " + (String(A).length < 2 ? "0" + A : A) + ":" + (String(B).length < 2 ? "0" + B : B);
	},
	dateDiff : function (B, A) {
		var _ = 24 * 60 * 60 * 1000,
		C = B + _ * 10 - parseInt(A);
		return Math.max((C / _).toFixed(0), 0);
	},
	useToast : function (B) {
		var E = {
			toastMode : B.toastMode,
			msg : B.msg,
			sticky : B.sticky,
			positionRB : B.position,
			closeType : B.closeType
		},
		_ = disk.Context.getService(disk.Context.SERVICE_TOAST);
		_.setVisible(true, E.sticky);
		_.setMessage(E.toastMode, E.msg);
		_.setClose(E.closeType);
		if (E.positionRB) {
			if (E.positionRB == disk.ui.Panel.TOP) {
				_.setGravity(disk.ui.Panel.TOP);
			} else {
				_.setPositionRB(0, 0);
			}
		} else {
			if (disk.Context.getService(disk.Context.SERVICE_CANVAS).isVisible()) {
				_.setGravity(disk.ui.Panel.CENTER);
			} else {
				try {
					if (!this._mFileListDOM) {
						this._mFileListDOM = $("#fileList").length ? $("#fileList") : $("body");
					}
					var G = this._mFileListDOM.offset(),
					D = (this._mFileListDOM.width() - _._mUI.pane.offsetWidth) / 2 + G.left,
					C = 50,
					A = 60;
					_.setLocation(D, A);
				} catch (F) {}

			}
		}
		return _;
	},
	useCloseToast : function () {
		var _ = disk.Context.getService(disk.Context.SERVICE_TOAST);
		_.setVisible(false);
	}
}, MoreOptionsDropDown = function () {
	disk.ui.Panel.call(this, this._prebuild());
	this._mMenuItems = null;
	this._mLastAttached = null;
	this._mLastAttachedData = null;
	this._mLastWrapper = null;
	this._mMenuContainer = null;
};
MoreOptionsDropDown.MENU_OPEN = 0;
MoreOptionsDropDown.MENU_DOWNLOAD = 1;
MoreOptionsDropDown.MENU_DEVICE = 2;
MoreOptionsDropDown.MENU_PRINT = 3;
MoreOptionsDropDown.MENU_SHARE = 4;
MoreOptionsDropDown.MENU_MOVE = 5;
MoreOptionsDropDown.MENU_COPY = 6;
MoreOptionsDropDown.MENU_RENAME = 7;
MoreOptionsDropDown.MENU_DELETE = 8;
MoreOptionsDropDown.MENU_EDIT_PIC = 9;
MoreOptionsDropDown.MENU_REVISION_LIST = 10;
MoreOptionsDropDown.MENU_SELL_FILE = 11;
MoreOptionsDropDown.MENU_SHOW_IN_FOLDER = 12;
MoreOptionsDropDown.MENU_CANCLE_SHARE = 0;
MoreOptionsDropDown.MENU_SHARE_LINK = 0;
MoreOptionsDropDown.MENU_CANCLE_PUBLIC = 1;
MoreOptionsDropDown.MENU_TRANSFER = 2;
MoreOptionsDropDown.MENU_RESTORE = 0;
MoreOptionsDropDown.MENU_PERSIST_REMOVE = 1;
MoreOptionsDropDown.singleton = null;
MoreOptionsDropDown.obtain = function () {
	if (MoreOptionsDropDown.singleton == null) {
		MoreOptionsDropDown.singleton = new MoreOptionsDropDown();
		MoreOptionsDropDown.singleton.init();
	}
	return MoreOptionsDropDown.singleton;
};
MoreOptionsDropDown.prototype = new disk.ui.Panel();
MoreOptionsDropDown.prototype._prebuild = function () {
	var _ = document.createElement("ul");
	_.className = "b-panel b-options-dd-menu b-list-drop b-bdr-7 bdr-rnd-2 box-shadow3";
	_.id = "right-context-menu";
	document.body.appendChild(_);
	return {
		pane : _
	};
};
MoreOptionsDropDown.prototype.installDeviceHanle = function () {
	$("#rightDeviceMenu").show();
	$("#rightDeviceMenu").mouseenter(function (A) {
		var _ = $(this).offset().left - $("#fileActionHeader").offset().left;
		if ($(document).width() - _ > 480) {
			_ += 121;
		} else {
			_ -= 112;
		}
		if ($("#netdiskTips:visible").length) {
			$(".device-menu").css({
				left : _,
				top : $(this).offset().top - 164
			}).show();
		} else {
			$(".device-menu").css({
				left : _,
				top : $(this).offset().top - 129
			}).show();
		}
		Page.obtain().installDevice();
	}).mouseleave(function () {
		$(".device-menu").hide();
	});
};
MoreOptionsDropDown.prototype._useBTTMode = function () {
	if (this._mMenuContainer == null) {
		this._mMenuContainer = document.getElementById("fileList");
	}
	var _ = this._mLastAttached;
	if (_) {
		var A = $(_).offset().top - $(this._mMenuContainer).offset().top + _.offsetHeight;
		if (A + this._mUI.pane.offsetHeight > this._mMenuContainer.offsetHeight) {
			return true;
		}
	}
	return false;
};
MoreOptionsDropDown.prototype._useReverseYMode = function (_) {
	if (this._mMenuContainer == null) {
		this._mMenuContainer = document.getElementById("fileList");
	}
	var A = _ - $(this._mMenuContainer).offset().top;
	if (A + $(this._mUI.pane).height() > $(this._mMenuContainer).height()) {
		return true;
	}
	return false;
};
MoreOptionsDropDown.prototype._useReverseXMode = function (_) {
	if (this._mMenuContainer == null) {
		this._mMenuContainer = document.getElementById("fileList");
	}
	var A = _ - $(this._mMenuContainer).offset().left;
	if (A + $(this._mUI.pane).width() > $(this._mMenuContainer).width()) {
		return true;
	}
	return false;
};
MoreOptionsDropDown.prototype.attach = function (Q, B, L) {
	var K = Q.childNodes[0].childNodes[0],
	F = Q.childNodes[0],
	I = Q,
	B = B,
	L = L;
	if (this._useReverseYMode(B)) {
		B = B - $(this._mUI.pane).height();
	}
	if (this._useReverseXMode(L)) {
		L = L - $(this._mUI.pane).width();
	}
	var P = this._mUI.pane;
	P.style.zIndex = "20";
	$(P).css({
		position : "absolute",
		top : B,
		left : L
	});
	var X = I.getAttribute("_position");
	if (X != null) {
		var R = parseInt(X),
		Y = FileUtils.getElementData(R);
		if (typeof Y != "undefined") {
			var V = FileUtils.getModule() == "category",
			U = Y.property || -1,
			C = FileUtils.inSearchMode() || FileUtils.isListViewLocked(),
			O = FileUtils.ifSpecialFolder(FileUtils.parseDirPath(Y.path || Y.typicalPath)),
			M = FileUtils.isInParticularDir(FileUtils.parseDirPath(Y.path || Y.typicalPath)),
			J = FileUtils.isTopParticularDir(FileUtils.parseDirPath(Y.path || Y.typicalPath)),
			Z = Y.isdir != 1 ? false : true,
			D = FileUtils.isRevisionCheck(FileUtils.getListViewCheckedItems()),
			S = FileUtils._checkIsBuyFile(Y.path),
			W = FileUtils.getListViewCheckedItems();
			FileUtils.SELECT_PIC_NUM = 0;
			var G = FileUtils.isAllPic(W[0].category);
			if (Page.inViewMode(Page.VIEW_HOME)) {
				if (W.length > 1) {
					for (var H = 0, _, A = W.length; H < A; H++) {
						_ = W[H];
						if (!U) {
							U = _.property == 0 || _.property == 2;
						}
						if (!O) {
							O = FileUtils.ifSpecialFolder(FileUtils.parseDirPath(_.path));
						}
						if (!M) {
							M = FileUtils.isInParticularDir(FileUtils.parseDirPath(_.path));
						}
						if (!J) {
							J = FileUtils.isTopParticularDir(FileUtils.parseDirPath(_.path));
						}
						if (!S) {
							S = FileUtils._checkIsBuyFile(_.path);
						}
						if (!Z) {
							Z = _.isdir != 1 ? false : true;
						}
						if (G) {
							G = FileUtils.isAllPic(_.category);
						}
					}
				}
				if (G) {
					this.setItemDisplay(MoreOptionsDropDown.MENU_EDIT_PIC, true);
					var T = $(this._mMenuItems[MoreOptionsDropDown.MENU_EDIT_PIC]);
					if (G == 1) {
						T.text("\u7f8e\u5316");
					} else {
						T.text("\u62fc\u56fe");
					}
				} else {
					this.setItemDisplay(MoreOptionsDropDown.MENU_EDIT_PIC, false);
				}
				this.setItemDisplay(MoreOptionsDropDown.MENU_SELL_FILE, false);
				this.setItemEnabled(MoreOptionsDropDown.MENU_DOWNLOAD, !C);
				this.setItemEnabled(MoreOptionsDropDown.MENU_PRINT, (G ? true : false));
				this.setItemEnabled(MoreOptionsDropDown.MENU_REVISION_LIST, (D ? true : false));
				this.setItemEnabled(MoreOptionsDropDown.MENU_RENAME, !C && U == -1 && !O && !J);
				this.setItemDisplay(MoreOptionsDropDown.MENU_SELL_FILE, false);
				this.setItemEnabled(MoreOptionsDropDown.MENU_PUBLIC, !C);
				this.setItemEnabled(MoreOptionsDropDown.MENU_DELETE, !C && !O && !J);
				this.setItemEnabled(MoreOptionsDropDown.MENU_SHARE, !C);
				this.setItemEnabled(MoreOptionsDropDown.MENU_COPY, !C && !O);
				this.setItemEnabled(MoreOptionsDropDown.MENU_MOVE, !C && U == -1 && !O && !J);
				this.setItemDisplay(MoreOptionsDropDown.MENU_SHOW_IN_FOLDER, W.length == 1 && (FileUtils.inSearchMode() || V));
				if (FileUtils.inSearchMode()) {
					this.setItemEnabled(MoreOptionsDropDown.MENU_DOWNLOAD, true);
					this.setItemEnabled(MoreOptionsDropDown.MENU_PUBLIC, true);
					this.setItemEnabled(MoreOptionsDropDown.MENU_SHARE, true);
					this.setItemEnabled(MoreOptionsDropDown.MENU_COPY, true);
					this.setItemEnabled(MoreOptionsDropDown.MENU_RENAME, U == -1 && !O && !J);
					this.setItemEnabled(MoreOptionsDropDown.MENU_MOVE, U == -1 && !O && !J);
					this.setItemEnabled(MoreOptionsDropDown.MENU_DELETE, !O && !J);
				}
				if (!G) {
					this.setItemDisplay(MoreOptionsDropDown.MENU_PRINT, false);
				}
				if (W.length > 1) {
					this.setItemDisplay(MoreOptionsDropDown.MENU_RENAME, false);
					this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, false);
				} else {
					if (W.length == 1) {
						this.setItemDisplay(MoreOptionsDropDown.MENU_RENAME, true);
						var E = W[0].path,
						N = "";
						if (E) {
							N = E.slice(E.lastIndexOf("."));
						}
						if (W[0].isdir || W[0].category == Category.Doc_oth || W[0].category == Category.PIC || W[0].category == Category.MUS || FileUtils.acceptOpenFileTypes(N)) {
							this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, true);
						} else {
							if (Page.inViewMode(Page.VIEW_HOME) && W[0].category == Category.MOV) {
								this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, true);
							} else {
								this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, false);
							}
						}
					}
				}
			} else {
				if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
					if (FileUtils.sysISSELF) {
						this.setItemDisplay(MoreOptionsDropDown.MENU_TRANSFER, false);
					}
					if (W.length > 1) {
						this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, false);
					} else {
						if (W.length == 1) {
							if (W[0].isdir || W[0].category == Category.Doc_oth || W[0].category == Category.PIC || W[0].category == Category.MUS || W[0].category == Category.MOV) {
								this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, true);
							} else {
								if (Page.inViewMode(Page.VIEW_HOME) && W[0].category == Category.MOV) {
									this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, true);
								} else {
									this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, false);
								}
							}
						}
					}
				} else {
					if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {}

				}
			}
			if (FileUtils.inSearchMode()) {
				this.setItemEnabled(MoreOptionsDropDown.MENU_DOWNLOAD, true);
				this.setItemDisplay(MoreOptionsDropDown.MENU_RENAME, true);
				this.setItemDisplay(MoreOptionsDropDown.MENU_PUBLIC, true);
				this.setItemDisplay(MoreOptionsDropDown.MENU_DELETE, true);
				this.setItemDisplay(MoreOptionsDropDown.MENU_SHARE, true);
				this.setItemDisplay(MoreOptionsDropDown.MENU_COPY, true);
				this.setItemDisplay(MoreOptionsDropDown.MENU_MOVE, true);
				if (W.length > 1) {
					this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, false);
				} else {
					if (W.length == 1) {
						this.setItemDisplay(MoreOptionsDropDown.MENU_OPEN, true);
					}
				}
			}
			this._mLastAttached = Q;
			this._mLastAttachedData = Y;
			this._mLastWrapper = F;
		} else {
			this._mLastAttached = null;
			this._mLastAttachedData = null;
			this._mLastWrapper = null;
		}
	}
	return this;
};
MoreOptionsDropDown.prototype.init = function () {
	if (Page.inViewMode(Page.VIEW_HOME)) {
		this._mUI.pane.innerHTML = '<li><a _i="0" hideFocus="true" href="javascript:;">\u6253\u5f00</a></li><li><a _i="1" hideFocus="true" href="javascript:;">\u4e0b\u8f7d</a></li><li id="rightDeviceMenu"><a _i="2" hideFocus="true" href="javascript:;">\u63a8\u9001\u5230\u8bbe\u5907\u25b6</a></li><li><a _i="3" hideFocus="true" href="javascript:;">\u4e91\u51b2\u5370</a></li><li><a _i="4" hideFocus="true" href="javascript:;">\u5206\u4eab</a></li><li><a _i="5" _data="move" hideFocus="true" href="javascript:;">\u79fb\u52a8\u5230</a></li><li><a _i="6" _data="copy" hideFocus="true" href="javascript:;">\u590d\u5236\u5230</a></li><li><a _i="7" hideFocus="true" href="javascript:;">\u91cd\u547d\u540d</a></li><li><a _i="8" hideFocus="true" href="javascript:;">\u5220\u9664</a></li><li><a _i="9" hideFocus="true" href="javascript:;">\u7f8e\u5316</a></li><li><a _i="10" hideFocus="true" href="javascript:;">\u5386\u53f2\u7248\u672c</a></li><li><a _i="11" hideFocus="true" href="javascript:;">\u51fa\u552e</a></li><li class="menu-separator b-bdr-7"><a _i="12" hideFocus="true" href="javascript:;">\u67e5\u770b\u6587\u4ef6\u5939</a></li>';
	} else {
		if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
			this._mUI.pane.innerHTML = '<li><a _i="0" hideFocus="true" href="javascript:;">\u53d6\u6d88\u5206\u4eab</a></li>';
		} else {
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				this._mUI.pane.innerHTML = '<li><a _i="0" hideFocus="true" href="javascript:;">\u6253\u5f00</a></li><li><a _i="1" hideFocus="true" href="javascript:;">\u4e0b\u8f7d</a></li><li><a _i="2" hideFocus="true" href="javascript:;">\u4fdd\u5b58</a></li>';
			} else {
				if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
					this._mUI.pane.innerHTML = '<li><a _i="0" hideFocus="true" href="javascript:;">\u8fd8\u539f</a></li><li><a _i="1" hideFocus="true" href="javascript:;">\u5f7b\u5e95\u5220\u9664</a></li>';
				}
			}
		}
	}
	var _ = this;
	this._mMenuItems = this._mUI.pane.getElementsByTagName("a");
	_.installDeviceHanle();
	$(this._mUI.pane).mousedown(function (_) {
		_.stopPropagation();
	});
	$(this._mUI.pane).click(function (F) {
		var B = F.target,
		D = null;
		if (B.nodeName.toLowerCase() == "a" && B.className.indexOf("disabled") == -1) {
			var C = B.getAttribute("_i");
			if (C == null) {
				throw new Error("Found Not Registered Menu Item");
			}
			C = parseInt(C);
			D = C;
			var E = _._mMenuItems[C];
			if (E) {
				_.dispatchMenuItemClick(C, F);
			}
			if (_._mLastWrapper != null) {
				$(_._mLastWrapper).removeClass("dd-hover");
			}
		} else {
			return false;
		}
		$(B).parent().parent().hide();
		var A = FileUtils.getListViewCheckedItems();
		if (!(A.length == 1 && !A[0].isdir && D == MoreOptionsDropDown.MENU_DOWNLOAD)) {
			if (F.preventDefault) {
				F.preventDefault();
			}
			if (F.stopPropagation) {
				F.stopPropagation();
			}
		}
		FileUtils._hideRightMouseMenu();
	});
};
MoreOptionsDropDown.prototype.dispatchMenuItemClick = function (B, C) {
	if (!this._mLastAttachedData) {
		return;
	}
	var _ = FileUtils.getListViewCheckedItems();
	if (Page.inViewMode(Page.VIEW_HOME)) {
		switch (B) {
		case MoreOptionsDropDown.MENU_OPEN:
			FileUtils.anchorFoldOpen(C, FileUtils, this._mLastAttached);
			break;
		case MoreOptionsDropDown.MENU_DOWNLOAD:
			Page.obtain().performSelectionDownload();
			break;
		case MoreOptionsDropDown.MENU_PRINT:
			Page.obtain().performSelectPrint();
			break;
		case MoreOptionsDropDown.MENU_REVISION_LIST:
			Page.obtain().performRevisionList();
			break;
		case MoreOptionsDropDown.MENU_RENAME:
			FileUtils.prepareFolderBuilder(FileUtils.UPDATE, this._mLastAttachedData, this._mLastAttached, true);
			break;
		case MoreOptionsDropDown.MENU_DELETE:
			Page.obtain().performSelectionDelete();
			break;
		case MoreOptionsDropDown.MENU_EDIT_PIC:
			Page.obtain().performSelectionEditPic("right");
			break;
		case MoreOptionsDropDown.MENU_SHARE:
			Page.obtain().performSelectionShareAll(_);
			break;
		case MoreOptionsDropDown.MENU_COPY:
			var A = Page.obtain().dispatchActionPerformed(Page.MENU_COPY, C.target);
			if (!A) {
				if (disk.DEBUG) {
					console.log("Suppress a download event, hand it over");
				}
				return false;
			}
			break;
		case MoreOptionsDropDown.MENU_MOVE:
			A = Page.obtain().dispatchActionPerformed(Page.MENU_MOVE, C.target);
			if (!A) {
				if (disk.DEBUG) {
					console.log("Suppress a download event, hand it over");
				}
				return false;
			}
			break;
		case MoreOptionsDropDown.MENU_SELL_FILE:
			Page.obtain().installSellDialog();
			break;
		case MoreOptionsDropDown.MENU_SHOW_IN_FOLDER:
			Page.obtain().performSelectionShowFolder();
			break;
		}
	}
	if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
		switch (B) {
		case MoreOptionsDropDown.MENU_CANCLE_SHARE:
			if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
				Page.obtain().performSelectionCancelShare();
			}
			break;
		}
	}
	if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
		switch (B) {
		case MoreOptionsDropDown.MENU_OPEN:
			FileUtils.anchorFoldOpen(C, FileUtils, this._mLastAttached);
			break;
		case MoreOptionsDropDown.MENU_DOWNLOAD:
			Page.obtain().performSelectionDownload();
			break;
		case MoreOptionsDropDown.MENU_TRANSFER:
			Page.obtain().performSelectionTransfer();
			break;
		}
	}
	if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
		switch (B) {
		case Page.MENU_RESTORE:
			Page.obtain().performSelectionRestore();
			return true;
		case Page.MENU_PERSIST_REMOVE:
			Page.obtain().performSelectionRemoval();
			return true;
		}
	}
};
MoreOptionsDropDown.prototype.setItemEnabled = function (_, B) {
	if (this._mMenuItems.length == 0) {
		this.init();
	}
	var A = this._mMenuItems[_];
	if (!A) {
		if (disk.DEBUG) {
			console.log("Try To Set Menu Item At ", _, " and menu length ", this._mMenuItems.length);
		}
		return;
	}
	A.className = B ? "" : "disabled";
	if (B) {
		this.setItemDisplay(_, B);
	}
};
MoreOptionsDropDown.prototype.setItemDisplay = function (_, B) {
	if (this._mMenuItems.length == 0) {
		this.init();
	}
	var A = this._mMenuItems[_];
	if (!A) {
		if (disk.DEBUG) {
			console.log("Try To Set Menu Item At ", _, " and menu length ", this._mMenuItems.length);
		}
		return;
	}
	$(A).parent().css("display", B ? "block" : "none");
};
var DiskFileFilter = function () {
	this._mCount = 0;
	this._mEligibles = {};
	this._mTag = null;
};
DiskFileFilter.MAX = 200;
DiskFileFilter.filter = function (B, A) {
	var D = null,
	E = 0;
	for (var _ = A.length, C = _ - 1; C >= 0; C--) {
		D = A[C].server_filename;
		if (D) {
			if (B.getEligibles() == 0) {
				break;
			}
			if (B.match(D)) {
				A.splice(C, 1);
			}
		}
		E++;
		if (E > DiskFileFilter.MAX) {
			break;
		}
	}
	if (disk.DEBUG) {
		console.log("kill all duplicate files with advanced count ", E);
	}
	return A;
};
DiskFileFilter.prototype = {
	clear : function () {
		if (disk.DEBUG) {
			console.log("drop out all filter eligibles");
		}
		if (this._mCount > 0) {
			this._mEligibles = {};
		}
		this._mCount = 0;
	},
	setTag : function (_) {
		this._mTag = _;
	},
	getTag : function () {
		return this._mTag;
	},
	addRule : function (_) {
		if (_ == null || _.length == 0) {
			return false;
		}
		if (typeof this._mEligibles[_] == "undefined") {
			if (disk.DEBUG) {
				console.log("add new filter file ", _);
			}
			this._mEligibles[_] = true;
			this._mCount++;
			return true;
		}
		return false;
	},
	removeRule : function (_) {
		if (_ == null || _.length == 0) {
			return false;
		}
		if (typeof this._mEligibles[_] != "undefined") {
			delete this._mEligibles[_];
			this._mCount--;
		}
		return false;
	},
	hasEligibles : function () {
		return this._mCount > 0;
	},
	getEligibles : function () {
		return this._mCount;
	},
	match : function (_) {
		if (this._mCount == 0) {
			return false;
		}
		if (_ == null || _.length == 0) {
			return false;
		}
		if (typeof this._mEligibles[_] != "undefined") {
			if (disk.DEBUG) {
				console.log("kill a eligible filter in the pool:", _);
			}
			delete this._mEligibles[_];
			this._mCount--;
			return true;
		}
		return false;
	}
};
var FileUtils = {
	ACTION_DELETE : 0,
	ACTION_COPY : 1,
	ACTION_MOVE : 2,
	ACTION_RENAME : 3,
	ACTION_SHARE : 4,
	ACTION_PUBLIC : 5,
	ACTION_UPLOAD : 6,
	ACTION_PRINT : 8,
	PART_TOP_FOLDER : 1,
	PART_KID_FOLDER : 2,
	PART_KID_IMAGE : 3,
	USER_VIEW_MODE : 0,
	FLAG_USER_CHANGED_MODE : false,
	SELECT_PIC_NUM : 0,
	SHARE_PUBLIC_LINK : 1,
	SHARE_PRIVATE_LINK : 0,
	SHARE_CHANNEL_PUBLIC_LINK : 0,
	SHARE_CHANNEL_PUBLIC_SNS : 1,
	PARTICULAR_DIR_MAP : {
		"/apps" : "/\u6211\u7684\u5e94\u7528\u6570\u636e",
		"/\u767e\u5ea6\u4e91\u6536\u85cf" : "/\u767e\u5ea6\u4e91\u6536\u85cf",
		"/\u6765\u81eaPC\u7684\u5907\u4efd\u6587\u4ef6" : "/\u6765\u81eaPC\u7684\u5907\u4efd\u6587\u4ef6",
		"/\u6211\u7684\u5206\u4eab" : "/\u6211\u7684\u5206\u4eab",
		"/\u6211\u7684\u4f5c\u54c1" : "/\u6211\u7684\u4f5c\u54c1"
	},
	quota : {
		total : "0",
		used : "0",
		free : "null",
		expire : "false"
	},
	onPersonalPageTabChange : function (F, C) {
		var A,
		E = parseInt($("span.sharenum-arrow").outerWidth() / 2);
		$("a.publiccnt").parent().removeClass("on-publiccnt on-concerncnt on-fanscnt");
		var D = FileUtils.sysUK_Master || FileUtils.sysUK,
		B = Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) ? "Ta" : "\u6211",
		_ = !(Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) && FileUtils.sysUK_Master !== FileUtils.sysUK) - 0;
		if (F == "share") {
			A = $(".publiccnt").offset().left;
			if (Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				$("a.publiccnt").parent().addClass("on-publiccnt");
			}
			setTimeout(function () {
				$("#followPanel iframe").attr("src", "").parent().css("display", "none");
			}, 10);
			$("span.sharenum-arrow").css({
				left : A,
				width : $("a.publiccnt").width()
			});
		}
	},
	getFlagHaveBTPlugs : function () {
		return true;
	},
	acceptOpenFileTypes : function (_) {
		if (!Page.inViewMode(Page.VIEW_HOME)) {
			return false;
		}
		_ = _ || "";
		_ = _.toLowerCase();
		var A = false;
		if (_ == ".torrent") {
			if (FileUtils.getFlagHaveBTPlugs()) {
				A = true;
			}
		} else {
			if (_ == ".zip" || _ == ".rar") {
				A = true;
			} else {
				if (_ == ".html" && (FileUtils.getModule() != "category" && (!FileUtils.inSearchMode()))) {
					var B = FileUtils.getDirMgr().getDir();
					if (B == "/\u767e\u5ea6\u4e91\u6536\u85cf/\u6587\u7ae0" || B.indexOf("/\u767e\u5ea6\u4e91\u6536\u85cf/\u6587\u7ae0/") == 0) {
						A = true;
					}
				} else {
					if (_ == ".dws" || _ == ".dwt" || _ == ".dxf" || _ == ".dwg") {
						A = true;
					}
				}
			}
		}
		return A;
	},
	sliceDir : function (_) {
		if (_ == "/") {
			_ = "";
		}
		return _.split("/");
	},
	removeFormatForPathName : function (_) {
		_ = _ || "";
		return _.replace(/\n|\r|\t/g, "");
	},
	getNormalizedPathFromFolder : function (A, _) {
		if (A.indexOf("/") != 0) {
			A = "/" + A;
		}
		return this.getNormalizedPath(A, _);
	},
	getNormalizedPath : function (F, C) {
		var E = this.PARTICULAR_DIR_MAP,
		G = E[F],
		_ = G;
		if (typeof G == "undefined") {
			var B = this.sliceDir(F),
			A = "/" + B[1],
			D = E[A];
			if (typeof D != "undefined") {
				_ = F.replace(A, D);
			} else {
				_ = F;
			}
		}
		return C === true ? this.parseDirFromPath(_) : _;
	},
	parseLongFileName : function (E, B, C) {
		var D = 30;
		if (typeof C != "undefined") {
			D = C;
		}
		var J = "";
		if (!B && E.lastIndexOf(".") != -1) {
			J = E.substring(E.lastIndexOf("."));
		}
		var F = 0,
		K = "",
		H = "";
		for (var G = 0, _ = E.length; G < _; G++) {
			if (E.charCodeAt(G) > 255) {
				F += 2;
			} else {
				F += 1;
			}
			if (F >= (D - J.length - 6 - 3) && K.length == 0) {
				K = E.substring(0, G + 1);
			}
			if (F > (D - J.length - 3)) {
				var I = "",
				M = E;
				if (E.lastIndexOf(".") != -1) {
					M = E.substring(0, E.lastIndexOf("."));
				}
				var A = 0;
				for (var L = M.length; L > M.length - 6; L--) {
					if (M.charCodeAt(L) > 255) {
						A++;
					}
					I = M.substring(L, L + 1) + I;
					if (A >= 3) {
						break;
					}
				}
				H = K + "..." + I + J;
			}
		}
		if (F > D) {
			return H;
		}
		return E;
	},
	isTopParticularDir : function (C) {
		var A = this.sliceDir(C),
		B = this.PARTICULAR_DIR_MAP,
		_ = "/" + A[1];
		return A.length <= 2 && (typeof B[_] != "undefined") && _ != "/\u6211\u7684\u4f5c\u54c1";
	},
	isAllPic : function (_) {
		if (_ == 3) {
			this.SELECT_PIC_NUM++;
		} else {
			this.SELECT_PIC_NUM = 0;
		}
		return this.SELECT_PIC_NUM;
	},
	isRevisionCheck : function (_) {
		if (_.length == 1 && _[0].category == 4) {
			return true;
		} else {
			return false;
		}
	},
	isInParticularDir : function (C) {
		var A = this.sliceDir(C),
		B = this.PARTICULAR_DIR_MAP,
		_ = "/" + A[1];
		return A.length >= 2 && (typeof B[_] != "undefined");
	},
	actionDenied : function (E, F, A) {
		var C = this.sliceDir(F),
		D = this.PARTICULAR_DIR_MAP,
		B = "/" + C[1],
		_ = C.length;
		if (_ >= 2 && (typeof D[B] != "undefined")) {
			switch (E) {
			case FileUtils.ACTION_DELETE:
				return _ <= 2;
			case FileUtils.ACTION_COPY:
				return !FileUtils.isInParticularDir(A);
			case FileUtils.ACTION_RENAME:
			case FileUtils.ACTION_MOVE:
				return true;
			case FileUtils.ACTION_SHARE:
				return false;
			case FileUtils.ACTION_PUBLIC:
				return false;
			case FileUtils.ACTION_UPLOAD:
				return true;
			default:
				return false;
			}
		} else {
			return false;
		}
	},
	uniqueFileList : function (D) {
		var E = [],
		C = {};
		for (var B = 0, A, _ = D.length; B < _ && (A = this.parseDirFromPath(D[B].path).toLowerCase()) != ""; B++) {
			if (!C[A]) {
				E.push(D[B]);
				C[A] = true;
			}
		}
		return E;
	},
	MODULE_DIR : "dir",
	MODULE_SEARCH : "s",
	MODULE_NOTI : "noti",
	MODULE_CATEGORY : "category",
	MODULE_LOCATION : "location",
	URLPATH : "/netdisk/",
	TYPE_QUERY : ["categoryinfo", "webcategorylist"],
	TYPE_PARAM : "category",
	TYPES : [[1, "\u97f3\u4e50"], [2, "\u56fe\u7247"], [3, "\u89c6\u9891"], [4, "\u6587\u6863"], [5, "\u5e94\u7528"], [6, "\u5176\u5b83"], [7, "\u79cd\u5b50"]],
	TYPESNAMEHASH : [0, 3, 1, 2, 4, 5, 6, 7],
	UPDATE : 0,
	CREATE : 1,
	SORT_BY_NAME : 0,
	SORT_BY_SIZE : 1,
	SORT_BY_TIME : 2,
	SORT_BY_REMAINING : 3,
	SORT_BY_VCNT : 4,
	SORT_BY_TCNT : 5,
	SORT_BY_DCNT : 6,
	MODE_NORMAL : 0,
	MODE_SEARCH : 1,
	ROOT_ID : 0,
	VIEW_LIST : 0,
	VIEW_GRID_ICON : 1,
	VIEW_GRID_THUMB : 2,
	_mDropdownDrawerFlag : -1,
	_firstEnter : true,
	_lastclick : null,
	_mSortColsDisplayed : true,
	_mLayoutMode : 0,
	_mLastSearchKey : null,
	_mPrecedeSortHandler : null,
	_mFileListDOM : null,
	_mFileGridDOM : null,
	_mDirMgr : null,
	_mLocationDOM : null,
	_mSelection : null,
	_mPendingHighlights : null,
	_mHighlightTimer : null,
	_mPendingHighlightIndexes : [],
	_mPendingHighlightFirst : -1,
	_mModule : null,
	_mType : null,
	_mLocalCache : new disk.util.FileCache(),
	_mPropertyCache : new disk.util.LocalCache(),
	_mPendingRequestShareProperty : null,
	_mSpecialFolders : new Array("/\u6211\u7684\u6587\u6863", "/\u6211\u7684\u7167\u7247", "/\u6211\u7684\u97f3\u4e50", "/\u6211\u7684\u89c6\u9891"),
	_mPropertyNode : null,
	_mInfiniteListView : null,
	_mInfiniteGridView : null,
	_mSortIndicatorCache : {},
	_mSortTriggers : [],
	_mQuerying : false,
	_mGridStep : 5,
	_mLastViewMode : 0,
	_mViewMode : 0,
	_mMboxPlayer : null,
	_mMboxPlayerIndex : 536000,
	_mDocumentReader : null,
	_mImgPreviewer : null,
	_mPushDevice : null,
	_mHtmlTarget : null,
	_mCloudPrint : null,
	_mRevisionList : null,
	_mOfflineList : null,
	_mNewOffline : null,
	_mResolveBTOffline : null,
	_mResolveUNZip : null,
	_mDraging : false,
	_mDragIcon : null,
	_mMoveSound : null,
	_mUsePBPLoading : false,
	_mPBPPage : -1,
	_mScheduleProxyLoading : false,
	_mProxyLoader : null,
	_mProxyCallback : null,
	_mCategoryProxyLoader : null,
	_mPendingPageSize : -1,
	_mProxySortingBy : ["time", "1"],
	_mHideLoadingToast : false,
	_mDataAwardIntrCache : null,
	_mRecommendBtPanel : null,
	_mCommentPanel : null,
	bImgReady : true,
	AWARD_DIALOG_INSTALL : "award_dialog_install",
	_mFileFilter : (new DiskFileFilter()),
	_mLastQueryTerms : null,
	getFileFilter : function () {
		return this._mFileFilter;
	},
	FLAG_COMING_BY_SEARCH : 0,
	_mMessageFromList : false,
	_mViewModeSwitchWatcher : null,
	hideLoadingToast : function (_) {
		this._mHideLoadingToast = _;
		if (disk.DEBUG) {
			console.log("assign hiding toast = ", _);
		}
	},
	getModule : function () {
		return this._mModule;
	},
	sendUserReport : function (_) {
		var A = disk.api.RestAPI.USER_REPORT;
		$.ajax(A, {
			type : "POST",
			data : {
				timestamp : Math.round(new Date / 1000),
				action : disk.getParam("from") || _ || "web_home"
			}
		});
	},
	sendAwardTaskRequest : function (_) {
		var A = {
			share_sina : 10,
			upload : 6
		};
		if (_ == "upload") {
			FileUtils._mDataAwardIntrCache = 1;
		}
		if (typeof localStorage != "undefined" && !(localStorage.getItem(_ + "_" + FileUtils.sysUID.replace(/@/g, "")))) {
			$.ajax({
				type : "POST",
				timeout : 3000,
				url : disk.api.RestAPI.normalize(disk.api.RestAPI.TASK_CREATE, FileUtils.bdstoken),
				dataType : "json",
				data : {
					type : A[_],
					sign : FileUtils.taskKey,
					timestamp : FileUtils.taskTime
				},
				success : function (B) {
					if (B && (B.errno == 0 || B.errno == -36 || B.errno == -29)) {
						if (disk.DEBUG) {
							console.log("..........type=" + _ + "data.errno" + B.errno);
						}
						localStorage.setItem(_ + "_" + FileUtils.sysUID.replace(/@/g, ""), A[_]);
					}
				}
			});
		} else {
			return false;
		}
	},
	showAwardIntrDialog : function (C) {
		if (C && C.task_detail) {
			FileUtils._mDataAwardIntrCache = C.task_detail[6];
		}
		var A = 5 * 1024 * 1024 * 1024,
		_ = null,
		B = {
			newtext : '\u4f60\u5df2\u6709<em class="nu">5G</em>\u6c38\u4e45\u514d\u8d39\u5bb9\u91cf\uff0c\u5b8c\u6210\u4efb\u52a1\u53ef\u8fbe<em class="nu">15G</em>\u54e6\uff01',
			oldtext : '\u514d\u8d39\u4efb\u52a1\u6709\u65b0\u4efb\u52a1\u5566\uff0c<br/>\u5b8c\u6210\u4efb\u52a1\u53ef\u8fbe<em class="nu">15G</em>\u54e6\uff01'
		};
		if (C.task_quota >= C.total_quota) {
			return;
		} else {
			if (C.task_quota <= A) {
				_ = new disk.ui.AwardDialog();
			} else {
				if (A < C.task_quota && C.task_quota < C.total_quota) {
					_ = new disk.ui.AwardIntr({
							msg : B.oldtext
						});
				}
			}
		}
		_.localStorageKey = FileUtils.AWARD_DIALOG_INSTALL + "_" + FileUtils.sysUID.replace(/@/g, "");
		_.localStorageVal = 1;
	},
	checkCategoryProxyLoaderIntegrality : function () {
		var A = this._mCategoryProxyLoader;
		if (A != null) {
			if (!A.isIdle()) {
				var _ = A.getTag();
				if (disk.DEBUG) {
					console.log("[1]Drop All Category Cache from category " + _);
				}
				if (_ != null) {
					FileUtils.getLocalCache().removeCategorys();
				}
			}
			A.setTag(null);
			A.reset();
		}
	},
	proxyCategoryLoad : function (A, _) {
		var C = this._mCategoryProxyLoader,
		E = this;
		if (C != null) {
			if (!C.isIdle()) {
				var D = C.getTag();
				if (D != null) {
					if (disk.DEBUG) {
						console.log("[2]Drop All Category Cache from category " + D);
					}
					FileUtils.getLocalCache().removeCategorys();
				}
			}
			C.setTag(null);
			C.reset();
		}
		if (C == null) {
			C = new disk.util.RestfulDataLoader(disk.api.RestAPI.LIST_CATEGORY + "&" + E.TYPE_PARAM + "=%s&pri=%s&num=100", "get", null, null);
			C.onDataAvailable = function (B, D) {
				var A = this.getTag();
				if (A != null) {
					FileUtils.getLocalCache().removeCategorys();
					E.triggerTypeUi(false);
					FileUtils._mDirMgr._mAfterSearch = false;
					FileUtils._mDirMgr.cd("", false, true);
					var G = '\u5206\u7c7b:"' + E.TYPES[A - 1][1] + '"';
					FileUtils.getLocalCache().add(G, D);
					FileUtils._mDirMgr.cd(G, false, B.length != D.length);
					FileUtils._mDirMgr._mAfterSearch = true;
				}
				if (B.length == D.length) {}
				else {
					switch (FileUtils._mViewMode) {
					case FileUtils.VIEW_GRID_ICON:
					case FileUtils.VIEW_GRID_THUMB:
						var _ = E.setGroupData(D, E._mGridStep);
						E._mInfiniteGridView.updateBackedData(_, true);
						E.changeProperty();
						break;
					case FileUtils.VIEW_LIST:
					default:
						E._mInfiniteListView.updateBackedData(FileUtils.getLocalCache().obtain(G), true);
						E.changeProperty();
						break;
					}
				}
				var H = true;
				if (this.hasMorePage()) {
					var F = E._mViewMode == FileUtils.VIEW_LIST ? E._mInfiniteListView : E._mInfiniteGridView,
					C = F.getScrollbar();
					if (C == null || !C.isAwake()) {
						this.load();
						H = false;
					}
				} else {
					this.setTag(null);
				}
				if (H) {
					E.shutdownNonListMessageToast();
				}
			};
			C.onDataUnavailable = function (C, D, A) {
				disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
				var _ = this.getCachedListData(),
				B = (_.length > 0 ? "\u90e8\u5206" : "") + "\u5217\u8868\u52a0\u8f7d\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5";
				if (E._mViewMode == FileUtils.VIEW_LIST) {
					if (E._mInfiniteListView != null) {
						E._mInfiniteListView.setBackedData([]);
					} else {
						$("#infiniteListView .loading-data").text(B);
					}
				} else {
					if (E._mInfiniteGridView != null) {
						E._mInfiniteGridView.setBackedData([]);
					} else {
						$("#infiniteGridView .loading-data").text(B);
					}
				}
				if (A !== true) {}

			};
			C.onLoadingEvent = function (A) {
				E._mQuerying = A;
				var _ = this.getTag();
				if (A) {
					if (_ != null && !E._mHideLoadingToast) {
						$("#dirData .loadedDate").html("\u52a0\u8f7d" + E.TYPES[_ - 1][1] + "\u5206\u7c7b&hellip;");
					}
				} else {}

			};
			C.setPagingBaseOne(true);
			this._mCategoryProxyLoader = C;
		}
		C.setTag(A);
		var B = null;
		if (this._mProxySortingBy.length > 0) {
			if (FileUtils._mType && FileUtils._mProxySortingBy[0] == "time2") {
				FileUtils._mProxySortingBy[0] = "time";
			}
			B = {};
			B.order = this._mProxySortingBy[0];
			if (this._mProxySortingBy[0]) {
				B.desc = this._mProxySortingBy[1];
			}
		}
		C.setRequestData(B);
		C.setRequestArgReplacements([Category.HASH[A], _]);
		C.setNeedLoadMorePage(true);
		FileUtils.getLocalCache().removeAll();
		C.load();
		return true;
	},
	checkProxyLoaderIntegrality : function () {
		var _ = this._mScheduleProxyLoading,
		B = this._mProxyLoader;
		if (_ && B != null) {
			if (!B.isIdle()) {
				var A = B.getTag();
				if (A != null) {
					if (disk.DEBUG) {
						console.log("1------>DROP OUT CACHE AT " + A);
					}
					FileUtils.getLocalCache().remove(A);
				}
			}
			B.setTag(null);
			B.reset();
		}
	},
	figureOutPageSizeFromPendingFiles : function () {
		var _ = 100;
		if (this._mPendingHighlights != null && this._mPendingHighlights.length > _) {
			this._mPendingPageSize = this._mPendingHighlights.length;
		}
	},
	issueProxyLoading : function (A, _) {
		this._mScheduleProxyLoading = A;
		if (_ === true) {
			this.figureOutPageSizeFromPendingFiles();
			if (FileUtils._mType) {
				FileUtils._mProxySortingBy[0] = "time";
			} else {
				FileUtils._mProxySortingBy[0] = "time2";
			}
			FileUtils._mProxySortingBy[1] = 1;
		}
	},
	shutdownNonListMessageToast : function () {
		var _ = this._mMessageFromList;
		this._mMessageFromList = false;
		if (!_) {
			disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
		}
	},
	proxyLoad : function (D, G, A) {
		var H = this,
		_ = this._mScheduleProxyLoading;
		this._mScheduleProxyLoading = false;
		var C = 100,
		F = C;
		if (this._mPendingPageSize != -1) {
			F = this._mPendingPageSize;
			this._mPendingPageSize = -1;
		}
		var E = this._mProxyLoader;
		if (E != null) {
			if (!E.isIdle()) {
				G = E.getTag();
				if (G != null) {
					FileUtils.getLocalCache().remove(G);
					if (disk.DEBUG) {
						console.log("2------>DROP OUT CACHE AT " + G);
					}
				}
			}
			E.setTag(null);
			this._mProxyCallback = null;
			E.reset();
		}
		if (!_) {
			return false;
		} else {
			if (E == null) {
				var B = disk.api.RestAPI.LIST;
				if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
					B = disk.api.RestAPI.RECORD_SHARE;
				} else {
					if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
						B = disk.api.RestAPI.LIST_SHARE;
					} else {
						if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
							B = disk.api.RestAPI.LIST_RECYCLE;
						} else {
							if (Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
								B = disk.api.RestAPI.LIST_SHARE_HOME + "&uk=" + FileUtils.share_uk;
							}
						}
					}
				}
				E = new disk.util.RestfulDataLoader(B + "&num=%s", "get", null, null);
				E.onDataAvailable = function (B, D) {
					var F = this.getTag();
					if (F != null) {
						FileUtils.getLocalCache().update(F, D, true);
					}
					if (B.length == D.length) {
						FileUtils.onBeforeRenderingListView(B);
						if (typeof H._mProxyCallback == "function") {
							H._mProxyCallback.call(H, D);
						} else {
							H.load(F);
						}
					} else {
						var A = FileUtils.getFileFilter();
						if (A.hasEligibles()) {
							DiskFileFilter.filter(A, D);
						}
						switch (FileUtils._mViewMode) {
						case FileUtils.VIEW_GRID_ICON:
						case FileUtils.VIEW_GRID_THUMB:
							var _ = H.setGroupData(D, H._mGridStep);
							H._mInfiniteGridView.updateBackedData(_, true);
							H.changeProperty();
							break;
						case FileUtils.VIEW_LIST:
						default:
							H._mInfiniteListView.updateBackedData(D, true);
							H.changeProperty();
							break;
						}
					}
					var G = true;
					if (this.hasMorePage()) {
						var E = H._mViewMode == FileUtils.VIEW_LIST ? H._mInfiniteListView : H._mInfiniteGridView,
						C = E.getScrollbar();
						if (C == null || !C.isAwake()) {
							this.load();
							G = false;
						}
					} else {
						this.setTag(null);
					}
					if (G) {
						H.shutdownNonListMessageToast();
					}
				};
				E.onDataUnavailable = function (C, D, A) {
					disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
					var _ = this.getCachedListData(),
					B = (_.length > 0 ? "\u90e8\u5206" : "") + "\u5217\u8868\u52a0\u8f7d\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5";
					if (H._mViewMode == FileUtils.VIEW_LIST) {
						if (H._mInfiniteListView != null) {
							H._mInfiniteListView.setBackedData([]);
						} else {
							$("#infiniteListView .loading-data").text(B);
						}
					} else {
						if (H._mInfiniteGridView != null) {
							H._mInfiniteGridView.setBackedData([]);
						} else {
							$("#infiniteGridView .loading-data").text(B);
						}
					}
					if (A !== true) {}

				};
				E.onLoadingEvent = function (_) {
					H._mQuerying = _;
					if (_ && !H._mHideLoadingToast) {
						$("#dirData .loadedDate").html("\u83b7\u53d6\u66f4\u591a\u6570\u636e&hellip;");
					}
				};
				E.setPagingBaseOne(true);
				this._mProxyLoader = E;
			}
			E.setTag(G);
			if (this._mProxySortingBy.length > 0) {
				D.order = this._mProxySortingBy[0];
				if (this._mProxySortingBy[1]) {
					D.desc = this._mProxySortingBy[1];
				}
			} else {
				D.order = this._mProxySortingBy[0];
				D.desc = this._mProxySortingBy[1];
			}
			E.setRequestData(D);
			E.setRequestArgReplacements([F]);
			this._mProxyCallback = A;
			E.setNeedLoadMorePage(true);
			this._mFileFilter.clear();
			E.load();
			return true;
		}
	},
	onBeforeRenderingListView : function (_) {
		if (disk.DEBUG) {
			console.log("do functions : onBeforeRenderingListView");
		}
		if (FileUtils.FLAG_USER_CHANGED_MODE == false) {
			if (Page.inViewMode(Page.VIEW_HOME)) {
				if (FileUtils.getModule() != "category" && (!FileUtils.inSearchMode())) {
					var A = FileUtils.getDirMgr().getDir();
					if (A == "/\u767e\u5ea6\u4e91\u6536\u85cf/\u56fe\u7247" || A.indexOf("/\u767e\u5ea6\u4e91\u6536\u85cf/\u56fe\u7247/") == 0) {
						if (this._mViewMode != FileUtils.VIEW_GRID_ICON) {
							FileUtils.switchViewMode(FileUtils.VIEW_GRID_ICON);
						}
					} else {
						if (this._mViewMode != FileUtils.VIEW_LIST) {
							FileUtils.switchViewMode(FileUtils.VIEW_LIST);
						}
					}
				}
			} else {
				if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
					if (this.judgeViewMode(_) == "gride") {
						if (this._mViewMode != FileUtils.VIEW_GRID_ICON) {
							FileUtils.switchViewMode(FileUtils.VIEW_GRID_ICON);
						}
					} else {
						if (this._mViewMode != FileUtils.VIEW_LIST) {
							FileUtils.switchViewMode(FileUtils.VIEW_LIST);
						}
					}
				}
			}
		}
	},
	judgeViewMode : function (_) {
		var A,
		E,
		C = [],
		D = "list",
		B;
		C.dir = C.noThumb = 0;
		for (A = 0, E = _.length; A < E; ++A) {
			B = _[A];
			if (B.isdir) {
				C.dir++;
			} else {
				if (!B.thumbs && !B.thumburl) {
					C.noThumb++;
				}
				C.push(B);
			}
		}
		if (C.noThumb / C.length <= 1 / 3) {
			D = "gride";
		}
		return D;
	},
	getProxyLoader : function () {
		return this._mProxyLoader;
	},
	getCategoryProxyLoader : function () {
		return this._mCategoryProxyLoader;
	},
	dispatchProxyLoading : function (_) {
		this._mScheduleProxyLoading = _;
	},
	setModule : function (_) {
		this._mModule = _;
	},
	getType : function () {
		return this._mType;
	},
	setType : function (_) {
		this._mType = _;
	},
	typeTable : function (_) {
		if (typeof t != "undefined") {
			this.getLocalCache()._typeTable = _;
			return this;
		}
		return this.getLocalCache()._typeTable;
	},
	triggerTypeUi : function (_) {
		$("#module-slide").css("left", "93px");
		$("#module-type").removeClass("unon");
		$("#module-path").addClass("unon");
		$("#choosetype").fadeOut();
	},
	getListViewCheckedItems : function () {
		var _ = this._mViewMode == FileUtils.VIEW_LIST ? this._mInfiniteListView : this._mInfiniteGridView;
		return _.getCheckedItems();
	},
	deactivateListView : function (A) {
		var _ = this._mViewMode == FileUtils.VIEW_LIST ? this._mInfiniteListView : this._mInfiniteGridView;
		if (_) {
			_.deactivate(A);
		}
	},
	isListViewFullyChecked : function () {
		if (this._mViewMode == FileUtils.VIEW_LIST) {
			return this._mInfiniteListView.getCheckedItems().length == this._mInfiniteListView.getCount();
		} else {
			return this._mInfiniteGridView.getCheckedItems().length == this._mInfiniteGridView.getCountEx();
		}
	},
	createCategoryUI : function (C, D, B) {
		var E = this;
		if (D == this.MODULE_DIR) {
			$("#tab-home").attr("href", "home");
			$("#module-slide").css("left", "2px");
			$("#module-path").removeClass("unon");
			$("#module-type").addClass("unon");
			$("#choosetype").fadeOut();
		} else {
			$("#module-slide").css("left", "93px");
			$("#module-type").removeClass("unon");
			$("#module-path").addClass("unon");
			$("#choosetype").fadeIn();
			var A = $("#type-size-allnumber").text(),
			_ = $("#type-sizebar").width();
			$(".type-num").html(function (_) {
				$("#type-num-" + (_ + 1)).html("(" + B[Category.HASH[_ + 1]].count + "\u4e2a)");
			});
			$(".type-size-num").html(function (_) {
				$("#type-size-" + (_ + 1)).html(FileUtils.toFriendlyFileSize(B[Category.HASH[_ + 1]].size));
			});
			$(".type-sizebar-a1").html(function (C) {
				$("#type-bar-" + (C + 1)).width(Math.ceil(B[Category.HASH[C + 1]].size / A * _)).css("borderLeftWidth", "0");
			});
		}
		$("#type-container .type-div .type-a").hover(function () {
			$(".type-folder", this).css("top", "-10px");
		}, function () {
			$(".type-folder", this).removeAttr("style");
		});
	},
	initAddSong : function (A) {
		if (this._mMboxPlayer == null || this._mMboxPlayer._mViewState) {
			return;
		}
		var _ = '<span class="b-in-blk sprite-ic b-ic-book" style="background-position: -253px -191px; "></span>';
		if (this._mMoveSound === null) {
			this._mMoveSound = $("<div>").attr("id", "add_song");
			this._mMoveSound.css({
				position : "absolute",
				"z-index" : 100
			});
		}
		this._mMoveSound[0].innerHTML = _ + A.innerHTML;
		this._mMoveSound.css({
			top : $(A).offset().top - 10,
			left : $(A).offset().left - 26
		});
		this._mMoveSound.appendTo("body").show();
		this._mMboxPlayer.startAni(this._mMoveSound, this._mMoveSound.offset().top, this._mMoveSound.offset().left);
	},
	initAddBackSong : function (A) {
		if (this._mMboxPlayer == null || this._mMboxPlayer._mViewState) {
			return;
		}
		var _ = '<div class="bubble-panel bubble-panel-back" id="bubbleAwardPanel" ><div class="bubble-arrow reverse"></div><div class="bubble-panel-main" id="bubbleMainContent">\u5df2\u52a0\u5165\u64ad\u653e\u5217\u8868</div></div>';
		if (this._mMoveSound === null) {
			this._mMoveSound = $("<div>").attr("id", "add_song");
			this._mMoveSound.css({
				position : "absolute",
				"z-index" : 100
			});
		}
		this._mMoveSound[0].innerHTML = _;
		this._mMoveSound.css({
			top : $(A).offset().top - 10,
			left : $(A).offset().left - 26
		});
		this._mMoveSound.appendTo("body").fadeIn("fast");
		this._mMboxPlayer.startAddBackSong(this._mMoveSound, this._mMoveSound.offset().top, this._mMoveSound.offset().left);
	},
	triggerDragFile : function (A) {
		var B = this;
		if (A === false) {
			$(document).unbind("mousemove", B.listenDragDocumentMove).unbind("mouseup", B.listenDragDocumentUp);
			if (B._mDragIcon) {
				B._mDragIcon.remove();
			}
			$("#infiniteListView").removeClass("dragging");
			B._mDraging = false;
			return;
		}
		B._mDraging = true;
		$("#infiniteListView").addClass("dragging");
		if (B._mDragIcon === null) {
			B._mDragIcon = $("<div>").attr("id", "drag_file_icon").html("<em></em>").hide();
			B._mDragIcon.mouseover(function (_) {
				B._mDragIcon.css({
					top : _.pageY + 1,
					left : _.pageX + 1
				});
			});
			if ($.browser.webkit) {
				B._mDragIcon.find("em").css({
					top : "11px",
					"-webkit-text-size-adjust" : "none"
				});
			}
		}
		B._mDragIcon.appendTo("body").hide();
		var _ = FileUtils.getListViewCheckedItems().length;
		if (_ > 9) {
			if (_ > 99) {
				_ = 99;
			}
			B._mDragIcon.find("em").text(_).css("left", "16px");
		} else {
			B._mDragIcon.find("em").text(_).css("left", "19px");
		}
		$(document).mousemove(function (_) {
			B.listenDragDocumentMove(_);
		}).mouseup(function (_) {
			B.listenDragDocumentUp(_);
		});
	},
	listenDragDocumentMove : function (_) {
		if (!_ || this._mDraging !== true) {
			return;
		}
		this._mDragIcon.css({
			top : _.pageY + 1,
			left : _.pageX + 1
		}).show();
	},
	listenDragDocumentUp : function (_) {
		if (!_ || this._mDraging === false) {
			return;
		}
		this.triggerDragFile(false);
	},
	triggerCategory : function (A) {
		return;
		this.setType(0);
		this.checkCategoryProxyLoaderIntegrality();
		var _ = this.getLocalCache(),
		B = this;
		Utilities.useToast({
			toastMode : disk.ui.Toast.MODE_LOADING,
			msg : "\u6b63\u5728\u52a0\u8f7d\u6570\u636e&hellip;",
			sticky : true
		});
		if (_._typeTable == null) {
			$.get(disk.api.RestAPI.INFO_CATEGORY + "&time=" + (new Date().getTime()), function (C) {
				try {
					result = $.parseJSON(C);
				} catch (D) {
					if (disk.DEBUG) {
						console.log("[LOG]====>parse json error on get data", e.message);
					}
				}
				if (result == null || result.errno != 0) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u7f51\u7edc\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
						sticky : false
					});
				} else {
					_._typeTable = result.info;
					toast.setVisible(false);
					B.createCategoryUI(B.getModule(), A, result.info);
					FileUtils._firstEnter = false;
					B.setModule(A);
				}
			});
		} else {
			this.createCategoryUI(this.getModule(), A, _._typeTable);
			this.setModule(A);
			toast.setVisible(false);
		}
		return this;
	},
	triggerType : function (B) {
		if (typeof B == "string" || B > 7 || B < 1) {
			return;
		}
		if (B == 1) {
			$("#barCmdPlayAll").removeClass("none");
		} else {
			$("#barCmdPlayAll").addClass("none");
		}
		this.setType(B);
		var A = this.getLocalCache(),
		E = B,
		F = this,
		D = '\u5206\u7c7b:"' + this.TYPES[B - 1][1] + '"',
		_ = -1;
		if (Page.inViewMode(Page.VIEW_HOME)) {}
		else {
			if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				_ = 0;
			}
		}
		F.triggerTypeUi(false);
		var C = this.getFlagTimeLine(B);
		this.resolveTimelineView(C);
		if (!A.obtain(D)) {
			this.deactivateListView(true);
			this.proxyCategoryLoad(B, _);
		} else {
			FileUtils._mDirMgr._mAfterSearch = false;
			FileUtils._mDirMgr.cd("", false, true);
			FileUtils._mDirMgr.cd(D, false);
			FileUtils._mDirMgr._mAfterSearch = true;
		}
		F.switchLayoutMode(FileUtils.MODE_NORMAL);
		this.clearCache.interrupt();
		this.clearCache.start();
		FileUtils.showTimelineGuide();
	},
	flagSendTimeLine : false,
	timeLineStatus : 0,
	getFlagTimeLineByCookie : function () {
		if (!this.flagIsFeatureTimeLine()) {
			return false;
		}
		if (FileUtils.timeLineStatus === true) {
			var A = ("timelinestatus_" + FileUtils.sysUID).replace(/@|\*/g, "");
			if (typeof localStorage !== "undefined" && typeof localStorage.getItem !== "undefined") {
				window.localStorage.setItem(A, 1);
			}
			FileUtils.flagSendTimeLine = true;
			FileUtils.updateTimeLineLinkUrl();
			return true;
		} else {
			if (FileUtils.timeLineStatus === false) {
				return false;
			}
		}
		if (typeof localStorage === "undefined" || typeof localStorage.getItem === "undefined" || ($.browser.msie && parseInt($.browser.version, 10) <= 6)) {
			return false;
		}
		var _ = localStorage.getItem(("timelinestatus_" + FileUtils.sysUID).replace(/@|\*/g, ""));
		if (_ == 1) {
			if (!FileUtils.flagSendTimeLine) {
				FileUtils.flagSendTimeLine = true;
				FileUtils.updateTimeLineLinkUrl();
			}
			return true;
		} else {
			if (!FileUtils.flagSendTimeLine) {
				FileUtils.flagSendTimeLine = true;
				FileUtils.updateTimeLineStatus();
			}
			return false;
		}
	},
	getFlagTimeLine : function (_) {
		var A = _ == 2 && FileUtils.getFlagTimeLineByCookie();
		return A;
	},
	removeTimelineView : function () {
		var _ = $("#panInner");
		_.removeClass("timeline-contains").parent().removeClass("timeline-contains-hide-tips");
		$("#timeline-tabs").hide();
		$(".location").show();
		$("#timeline-frame").remove();
	},
	resolveTimelineView : function (A) {
		FileUtils.removeTimelineView();
		if (!A) {
			return;
		}
		if ($("#timeline-tabs").length <= 0) {
			return;
		}
		var _ = $("#panInner");
		_.addClass("timeline-contains").parent().addClass("timeline-contains-hide-tips");
		$("#timeline-tabs").show();
		$(".location").hide();
		$(window).resize();
		setTimeout(function () {
			FileUtils._mDiskLog.send({
				type : "timeline_image_grid_view"
			});
		}, 2000);
	},
	timeLineGuideState : "0",
	flagIsFeatureTimeLine : function () {
		var _ = false;
		if (typeof disk.ui.Utilities.getBetaFeatureByTimeLine === "function") {
			_ = disk.ui.Utilities.getBetaFeatureByTimeLine();
		}
		return _;
	},
	showTimelineGuide : function () {
		$("#timeLineGuide").remove();
		if (!this.flagIsFeatureTimeLine()) {
			return;
		}
		if ($.browser.msie && parseInt($.browser.version, 10) <= 6) {
			return;
		}
		var _ = ("flag_timeline_guide_" + FileUtils.sysUID).replace(/@|\*/g, ""),
		A;
		if (typeof localStorage !== "undefined" && typeof localStorage.getItem !== "undefined") {
			A = window.localStorage.getItem(_);
		}
		if (A != 1 && FileUtils.getFlagTimeLineByCookie() == false && FileUtils.timeLineGuideState != "1" && (FileUtils.getModule() == null || FileUtils.getModule() === "category") && FileUtils.getType() === 2) {
			$(document.body).append('<div id="timeLineGuide"><a class="timeline-apply" href="javascript:;">\u7533\u8bf7\u4f18\u5148\u4f53\u9a8c</a><a class="timeline-close" href="javascript:;">\u5173\u95ed</a></div>');
			$("#timeLineGuide").delegate(".timeline-close", "click", function () {
				$("#timeLineGuide").remove();
				var _ = ("flag_timeline_guide_" + FileUtils.sysUID).replace(/@|\*/g, "");
				if (typeof localStorage !== "undefined" && typeof localStorage.getItem !== "undefined") {
					window.localStorage.setItem(_, 1);
				}
			}).delegate(".timeline-apply", "click", function () {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u7533\u8bf7\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
				FileUtils.updateTimelineGuide();
			});
			$("#timeLineGuide").css("top", $(".all-files-headers").offset().top - $("#timeLineGuide").height());
		}
	},
	updateTimeLineStatus : function () {
		$.getJSON("/api/image/getuinfo", function (_) {
			if (_ && _.errno == 0 && _.lt_flag == 2) {
				if (typeof localStorage !== "undefined" && typeof localStorage.setItem !== "undefined") {
					localStorage.setItem(("timelinestatus_" + FileUtils.sysUID).replace(/@|\*/g, ""), 1);
				}
				FileUtils.updateTimeLineLinkUrl();
			}
		});
	},
	updateTimeLineLinkUrl : function () {
		var _ = $(".b-list-3 a[cat=2]", "#aside");
		_.attr("href", "/disk/timeline");
	},
	updateTimelineGuide : function () {
		var _ = function () {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u7533\u8bf7\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
				sticky : false
			});
		};
		$.getJSON("/api/image/apply", function (A) {
			if (A && (A.errno == 0 || A.errno == 110)) {
				FileUtils.timeLineGuideState = "1";
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_SUCCESS,
					msg : "\u7533\u8bf7\u6210\u529f\uff0c\u4e24\u5929\u540e\u518d\u6765\u4f53\u9a8c\u65f6\u5149\u8f74\u529f\u80fd\u5427\uff01",
					sticky : false
				});
				$("#timeLineGuide").remove();
			} else {
				_();
			}
		}).error(function () {
			_();
		});
		FileUtils._mDiskLog.send({
			type : "timeline_image_apply"
		});
	},
	layout : function (_, B) {
		var C = this;
		if (_) {
			var A = null;
			return function () {
				if (A) {
					clearTimeout(A);
				}
				A = setTimeout(function () {
						Utilities.resolveLayout();
						disk.Context.EmbedApp.dispatchResizeWindow();
						if (FileUtils.isListViewLocked()) {
							if (FileUtils._mLocalCache.obtain(FileUtils.resolveCurrentDirKey()) && FileUtils._mLocalCache.obtain(FileUtils.resolveCurrentDirKey()).length > 2) {
								disk.ui.FolderBuilder.getDefault().dispatchNegativeAction();
							} else {
								return false;
							}
						}
						switch (FileUtils._mViewMode) {
						case FileUtils.VIEW_GRID_ICON:
						case FileUtils.VIEW_GRID_THUMB:
							if (FileUtils._mGridStep !== Math.floor($(".location:first").outerWidth() / 150) || 2) {
								FileUtils.switchViewMode(FileUtils.VIEW_GRID_ICON);
							}
							if (C._mInfiniteGridView != null) {
								C._mInfiniteGridView.requestLayout();
							}
							break;
						case FileUtils.VIEW_LIST:
						default:
							if (C._mInfiniteListView != null) {
								C._mInfiniteListView.requestLayout();
							}
							break;
						}
					}, B);
			};
		} else {
			return function () {
				Utilities.resolveLayout();
				disk.Context.EmbedApp.dispatchResizeWindow();
				switch (FileUtils._mViewMode) {
				case FileUtils.VIEW_GRID_ICON:
				case FileUtils.VIEW_GRID_THUMB:
					if (C._mInfiniteGridView != null) {
						C._mInfiniteGridView.requestLayout();
					}
					break;
				case FileUtils.VIEW_LIST:
				default:
					if (C._mInfiniteListView != null) {
						C._mInfiniteListView.requestLayout();
					}
					break;
				}
			};
		}
	},
	computeHidingStrategy : function (D) {
		if (!D) {
			return;
		}
		var C = FileUtils.parseDirPath(D),
		B = FileUtils.parseDirFromPath(D);
		if ("/" + B == C) {
			this._mDirMgr.setHidingInfo(0, "");
			return;
		}
		var _ = C.substring(0, C.lastIndexOf("/"));
		if (disk.DEBUG) {
			console.log("Base Path = ", _);
		}
		var A = _.substring(1).split("/").length;
		this._mDirMgr.setHidingInfo(A, _);
	},
	hitInHidingRoot : function (C) {
		var B = this._mLocalCache.obtain(FileUtils.ROOT_ID);
		for (var A = 0, _ = B.length; A < _; A++) {
			if (B[A].path == C) {
				return true;
			}
		}
		return false;
	},
	tryToUseHidingStrategy : function (D) {
		if (D == null) {
			return;
		}
		var E = D[1].path;
		if (typeof E == "undefined") {
			return;
		}
		var B = this._mLocalCache.obtain(FileUtils.ROOT_ID),
		C = null;
		for (var A = 0, _ = B.length; A < _; A++) {
			C = B[A].path;
			if (E.indexOf(C) == 0) {
				this.computeHidingStrategy(C);
			}
		}
	},
	sortBy : function (C, _) {
		if (disk.DEBUG) {
			console.log("[LOG]sort by ", C, " and reverse ", _);
		}
		var B = this.resolveCurrentDirKey();
		if (this._mLayoutMode == FileUtils.MODE_SEARCH && this._mLastSearchKey != null) {
			B = this._mLastSearchKey;
		}
		var A = this._mLocalCache.obtain(B);
		if (typeof A != "undefined") {
			if (this.inHomeMode()) {
				switch (C) {
				case FileUtils.SORT_BY_SIZE:
					this._mProxySortingBy[0] = "size";
					break;
				case FileUtils.SORT_BY_TIME:
					this._mProxySortingBy[0] = "time";
					break;
				default:
					this._mProxySortingBy[0] = "name";
					break;
				}
				this._mProxySortingBy[1] = _ === true ? 1 : 0;
				this._mScheduleProxyLoading = true;
				FileUtils.getLocalCache().removeAll();
				FileUtils.getLocalCache().removeCategorys();
				FileUtils.loadDir(FileUtils.resolveCurrentDirKey());
			} else {
				if (Page.inViewMode(Page.VIEW_HOME) && FileUtils.getModule() == "category") {
					switch (C) {
					case FileUtils.SORT_BY_SIZE:
						FileUtils._mProxySortingBy[0] = "size";
						break;
					case FileUtils.SORT_BY_TIME:
						FileUtils._mProxySortingBy[0] = "time";
						break;
					default:
						FileUtils._mProxySortingBy[0] = "name";
						break;
					}
					FileUtils._mProxySortingBy[1] = _ === true ? 1 : 0;
					this._mScheduleProxyLoading = true;
					FileUtils.getLocalCache().removeAll();
					FileUtils.getLocalCache().removeCategorys();
					this.triggerType(FileUtils.getType());
				} else {
					this._sortFiles(A, C, _);
					this.load(B);
				}
			}
		}
	},
	switchLayoutMode : function (_) {
		this._mLayoutMode = _;
	},
	inHomeMode : function () {
		return Page.inViewMode(Page.VIEW_HOME) && (this._mModule != "category");
	},
	inSearchMode : function () {
		return this._mLayoutMode == FileUtils.MODE_SEARCH;
	},
	_sortFiles : function (A, E, _) {
		if (!A) {
			if (disk.DEBUG) {
				console.log("sort ending... files does not exists. ");
			}
			return false;
		}
		var C = document.body.className,
		D = this.PARTICULAR_DIR_MAP,
		B = function (B, H) {
			var F = FileUtils.parseDirPath(B.path || B.typicalPath),
			G = FileUtils.parseDirPath(H.path || H.typicalPath);
			if (C != "flag-4") {
				if (typeof D[G] != "undefined") {
					return 1;
				} else {
					if (typeof D[F] != "undefined") {
						return -1;
					}
				}
			}
			if (!Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) && !Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				var A = H.isdir - B.isdir;
				if (A != 0) {
					return A;
				}
			}
			switch (E) {
			case FileUtils.SORT_BY_SIZE:
				if (typeof B.size == "undefined") {
					B.size = 0;
				}
				if (typeof H.size == "undefined") {
					H.size = 0;
				}
				A = B.size - H.size;
				break;
			case FileUtils.SORT_BY_TIME:
				A = (B.server_mtime || B.server_ctime || B.ctime || 0) - (H.server_mtime || H.server_ctime || H.ctime || 0);
				break;
			case FileUtils.SORT_BY_REMAINING:
				A = (B.dateDiff - H.dateDiff) || FileUtils.parseDirFromPath(B.path || B.typicalPath).localeCompare(FileUtils.parseDirFromPath(H.path || H.typicalPath));
				break;
			case FileUtils.SORT_BY_VCNT:
				A = B.vCnt - H.vCnt;
				break;
			case FileUtils.SORT_BY_TCNT:
				A = B.tCnt - H.tCnt;
				break;
			case FileUtils.SORT_BY_DCNT:
				A = B.dCnt - H.dCnt;
				break;
			case FileUtils.SORT_BY_NAME:
			default:
				A = FileUtils.parseDirFromPath(B.path || B.typicalPath).localeCompare(FileUtils.parseDirFromPath(H.path || H.typicalPath));
				break;
			}
			if (_ === true) {
				A = -A;
			}
			return A;
		};
		A.sort(B);
	},
	parseDirPath : function (_) {
		return _.substring(_.indexOf(":/") + 1);
	},
	parseDirFromPath : function (_) {
		return _.substring(_.lastIndexOf("/") + 1);
	},
	parseFullDirFromPath : function (_) {
		return _.substring(0, _.lastIndexOf("/"));
	},
	parseNameFromPath : function (_) {
		return _.substring(_.lastIndexOf("/") + 1, _.length);
	},
	ifSpecialFolder : function (A, _) {
		return false;
	},
	resolveDirKey : function (_) {
		return _ == disk.util.DirectoryLocation.DIR_ROOT ? FileUtils.ROOT_ID : _;
	},
	resolveCurrentDirKey : function () {
		if (this._mModule == "category") {
			return '\u5206\u7c7b:"' + this.TYPES[this._mType - 1][1] + '"';
		}
		return this.resolveDirKey(this._mDirMgr.getDir());
	},
	getLocalCache : function () {
		return this._mLocalCache;
	},
	getCategory : function () {
		return this._mCategory;
	},
	getDirMgr : function () {
		return this._mDirMgr;
	},
	scheduleQueryDir : function (D, H) {
		if (D && D.length > 4 && D.search(/:/) == 2) {
			var J = D.split(":");
			if (J[0] === "\u641c\u7d22") {
				Page.obtain().performGlobalSearch(J[1].substring(1, (J[1].length - 1)));
				return;
			} else {
				if (J[0] === "\u5206\u7c7b") {}

			}
		}
		this._mQuerying = true;
		var F = this;
		if (this._mUsePBPLoading) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_LOADING,
				msg : "\u6587\u4ef6\u6570\u91cf\u8f83\u591a\uff0c\u52a0\u8f7d\u53ef\u80fd\u9700\u8981\u4e00\u4e9b\u65f6\u95f4&hellip;",
				sticky : true
			});
		}
		var I = {
			dir : D == FileUtils.ROOT_ID ? "/" : D,
			t : Math.random().toFixed(5)
		};
		if (this._mPendingRequestShareProperty != null) {
			I.pri = this._mPendingRequestShareProperty;
			this._mPendingRequestShareProperty = null;
		}
		var A = document.body.className;
		if (A.indexOf("flag-2") != -1) {
			I.uk = FileUtils.share_uk;
			I.shareid = FileUtils.share_id;
		} else {
			if (A.indexOf("flag-3") != -1) {
				I.uk = FileUtils.sysUK;
			} else {
				if (A.indexOf("flag-5") != -1) {}

			}
		}
		var E = 2000,
		C = Page.inViewMode(Page.VIEW_HOME),
		_ = disk.api.RestAPI.LIST,
		G = C || Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) || Page.inViewMode(Page.VIEW_PROPERTY_OTHER) || Page.inViewMode(Page.VIEW_RECYCLE_BIN),
		K = Page.obtain();
		if (K && K._mMoveCopyDialog && K._mMoveCopyDialog.isVisible && typeof K._mMoveCopyDialog.isVisible === "function" && K._mMoveCopyDialog.isVisible() === true) {
			G = false;
			if (disk.DEBUG) {
				console.log("not remove cache becourse MoveCopyDialog is visible");
			}
		}
		if (G) {
			var B = this.proxyLoad(I, D, H);
			if (B) {
				if (disk.DEBUG) {
					console.log("[LOG]", "proxy loader inject, job accepted");
				}
				this._mQuerying = false;
				return true;
			}
			_ = disk.api.RestAPI.LIST + "&num=" + E;
			if (this._mUsePBPLoading) {
				_ += "&page=" + this._mPBPPage;
			} else {
				_ += "&page=1";
			}
		} else {
			if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
				_ = disk.api.RestAPI.LIST_RECYCLE;
			} else {
				if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
					_ = disk.api.RestAPI.LIST_SHARE + location.search;
				}
			}
		}
		$.get(_, I, function (_) {
			disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
			var A = null;
			try {
				A = $.parseJSON(_);
			} catch (B) {}

			if (A != null && parseInt(A.errno) == 0 && A.list != null) {
				if (F._mUsePBPLoading) {
					F._mPBPPage++;
					if (A.list.length > 0) {
						F._mLocalCache.appendList(D, A.list);
						F.loadDir(D, H);
						F.lockActiveRenderingView(true);
						F.scheduleQueryDir(D, H);
					} else {
						F._mUsePBPLoading = false;
						F._mPBPPage = -1;
						F.lockActiveRenderingView(false);
						F.loadDir(D, H);
						F._mQuerying = false;
					}
				} else {
					if (C && A.list.length >= E) {
						F._mUsePBPLoading = true;
						F._mPBPPage = 2;
						F.lockActiveRenderingView(true);
						F._mLocalCache.add(D, A.list);
						F.loadDir(D, H);
						F.scheduleQueryDir(D, H);
					} else {
						F._mLocalCache.add(D, A.list);
						F.loadDir(D, H);
						F._mQuerying = false;
					}
				}
			} else {
				if (F._mUsePBPLoading) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u90e8\u5206\u6587\u4ef6\u672a\u6210\u529f\u52a0\u8f7d\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
						sticky : true,
						closeType : true
					});
					F._mUsePBPLoading = false;
					F._mPBPPage = -1;
					F.lockActiveRenderingView(false);
					F.loadDir(D, H);
				} else {
					if (FileUtils.inSearchMode()) {
						disk.util.HistoryManager.getDefault();
					} else {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u90e8\u5206\u6587\u4ef6\u672a\u6210\u529f\u52a0\u8f7d\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
							sticky : true,
							closeType : true
						});
						if (!F._mDirMgr.atRoot()) {
							if (disk.DEBUG) {
								console.error("something wired happened");
							}
							disk.util.HistoryManager.getDefault().go(-1);
						} else {
							F._mLocalCache.add(D, []);
							F.loadDir(D, H);
						}
					}
				}
				F._mQuerying = false;
			}
		});
	},
	loadDir : function (B, A) {
		if (!this.bImgReady && FileUtils.getModule() != "category") {
			this._mLocalCache.removeAll();
			this.bImgReady = true;
		}
		this.checkProxyLoaderIntegrality();
		if (this._mSelectionArbitrate) {
			this._mSelectionArbitrate.removeClass("on");
		}
		var _ = FileUtils.getLocalCache()._pathByType;
		if (!this._mLocalCache.contains(B) || (_ && _[B])) {
			this.scheduleQueryDir(B, A);
		} else {
			if (typeof A == "function") {
				A.call(this, this._mLocalCache.obtain(B));
			} else {
				FileUtils.onBeforeRenderingListView(this._mLocalCache.obtain(B));
				this.load(B);
			}
		}
		this.clearCache.interrupt();
		this.clearCache.start();
	},
	clearCache : new disk.util.TimerService(Utilities._mInterval || 3600000, function () {
		if (FileUtils.isListViewLocked()) {
			disk.ui.FolderBuilder.getDefault().dispatchNegativeAction();
		}
		if (FileUtils.getModule() == "category") {
			if (FileUtils.getType()) {
				FileUtils.triggerType(FileUtils.getType() - 0);
			}
		} else {
			if (FileUtils._mDirMgr.atRoot() && !Page.inViewMode(Page.VIEW_HOME)) {
				if (disk.DEBUG) {
					console.log("Block Non-Home Root Refreshing");
				}
			} else {
				FileUtils.issueProxyLoading(true);
				FileUtils.loadDir(FileUtils.resolveCurrentDirKey());
			}
		}
		this.start();
	}),
	switchToDir : function (_) {
		this._mDirMgr.switchDir(disk.util.DirectoryLocation.toDirArray(_));
	},
	toCategorizedCrumb : function () {
		return '<li id="dirPath"><span class="dirCategory">\u5168\u90e8' + this.TYPES[this._mType - 1][1] + '</span><span id="dirLocation"></span></li><li id="dirData"><span><span class="loadedDate">-</span></span></li>';
	},
	buildDirMgr : function () {
		var _ = new disk.util.DirectoryLocation(),
		A = this;
		_.onDirChange = function (_) {
			if (A.isListViewLocked()) {
				return;
			}
			if (A._mPrecedeSortHandler != null) {
				if (Page.inViewMode(Page.VIEW_HOME) && FileUtils.getModule() == "category") {}
				else {
					$(A._mPrecedeSortHandler).removeClass("desc");
				}
			}
			A._mLocationDOM.html(FileUtils.getModule() != "category" ? this.toCrumbString(A._mLocationDOM.attr("label")) : A.toCategorizedCrumb());
			var B = function (D) {
				if (this.className.indexOf("disabled") != -1) {
					return false;
				}
				if (A._mModule == "category") {
					if (A._mQuerying) {
						return false;
					}
					var B = this.getAttribute("cat");
					if (B) {
						B = parseInt(B);
						if (B == A._mType) {
							return false;
						}
						A.deactivateListView(true);
						Page.obtain().performType(B - 1);
					} else {
						A.deactivateListView(true);
						disk.util.HistoryManager.getDefault().navigate(FileUtils.MODULE_CATEGORY, null);
					}
					return false;
				}
				A.switchLayoutMode(FileUtils.MODE_NORMAL);
				if (A.isListViewLocked()) {
					return false;
				}
				var _ = this.getAttribute("deep");
				if (disk.DEBUG) {
					console.assert(_ != null);
				}
				if (_) {
					_ = parseInt(_);
					if (_ < 0) {
						return false;
					}
					if (A._mQuerying) {
						return false;
					}
					A.deactivateListView(true);
					if (disk.DEBUG) {
						console.log(A._mDirMgr.getLookback(_), "<<<DIR");
					}
					var C = A._mDirMgr.getLookback(_);
					FileUtils.historyNavigate(C == disk.util.DirectoryLocation.DIR_ROOT ? {}

						 : {
						path : C
					});
					return false;
				}
			};
			$("a", A._mLocationDOM).each(function (A, _) {
				$(_).click(B);
			});
			A.loadDir(A.resolveDirKey(_));
		};
		_.setPathFilter(function (B) {
			var _ = A._mDirMgr.getDir();
			if (FileUtils.isInParticularDir(_)) {
				return A.getNormalizedPathFromFolder(B, true);
			} else {
				return B;
			}
		});
		this._mDirMgr = _;
		this._mLocationDOM = $(".dir-path:first");
		disk.util.HistoryManager.getDefault().listen(FileUtils.MODULE_DIR, function (B) {
			var _ = A._mFileFilter.getTag(),
			D = B.path;
			if (A._mFileFilter.hasEligibles()) {
				if (_ != null && _ != D) {
					A._mFileFilter.clear();
				}
			}
			A._mFileFilter.setTag(D || "/");
			if (FileUtils.isListViewLocked()) {
				disk.ui.FolderBuilder.getDefault().dispatchNegativeAction();
			}
			A.checkCategoryProxyLoaderIntegrality();
			if (disk.DEBUG) {
				console.log("Start to navigate to = ", B.path);
			}
			A._mScheduleProxyLoading = true;
			A.deactivateListView(true);
			FileUtils.createCategoryUI(FileUtils.getModule(), FileUtils.MODULE_DIR);
			FileUtils.setModule(FileUtils.MODULE_DIR);
			$("#barCmdPlayAll").addClass("none");
			$("#barCmdNewFolder").removeClass("none");
			if (Page.inViewMode(Page.VIEW_HOME)) {
				$("#tab-home").addClass("on");
				$("#aside-menu a.b-no-ln").removeClass("on");
			}
			FileUtils.switchLayoutMode(FileUtils.MODE_NORMAL);
			var C = B.path;
			if (typeof C != "undefined") {
				if (!Page.inViewMode(Page.VIEW_HOME) && FileUtils.hitInHidingRoot(C)) {
					FileUtils.computeHidingStrategy(C);
				}
				A.switchToDir(A.parseDirPath(C));
			} else {
				A._mDirMgr.cd(disk.util.DirectoryLocation.DIR_ROOT, false);
			}
		});
		window.onresize = FileUtils.layout($.browser, 50);
	},
	pendingHighlights : function (_) {
		this._mPendingHighlights = _;
	},
	cancelQuestionnaire : function (_) {
		_ && _();
		return null;
	},
	cancelShareInView : function (A) {
		var B = this,
		_ = disk.Context.getService(disk.Context.SERVICE_DIALOG);
		_.setVisible(true);
		$("#chunleireader").css("visibility", "hidden");
		_.setMessage("\u53d6\u6d88\u5206\u4eab", "\u53d6\u6d88\u5206\u4eab\u540e\uff0c\u8be5\u6761\u5206\u4eab\u8bb0\u5f55\u5c06\u88ab\u5220\u9664\uff0c\u597d\u53cb\u5c06\u65e0\u6cd5\u518d\u8bbf\u95ee\u6b64\u5206\u4eab\u94fe\u63a5\u3002<br/>\u4f60\u786e\u8ba4\u8981\u53d6\u6d88\u5206\u4eab\u5417\uff1f", function () {
			this.setVisible(false);
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u6b63\u5728\u5220\u9664\u5916\u94fe\uff0c\u6210\u529f\u540e\u5c06\u5173\u95ed\u7a97\u53e3&hellip;",
				sticky : true,
				closeType : true
			});
			$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.CANCEL_SHARE, FileUtils.bdstoken), A, function (C) {
				try {
					if (disk.DEBUG) {
						console.log("cancel share response:", C);
					}
					var A = $.parseJSON(C);
					if (A.errno == 0) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_SUCCESS,
							msg : "\u5916\u94fe\u5df2\u5220\u9664\u3002",
							sticky : false
						});
						B.cancelQuestionnaire(function () {
							try {
								window.opener = null;
								window.close();
							} catch (_) {}

							finally {
								window.location.reload();
							}
						});
					} else {
						var _ = disk.util.shareErrorMessage[A.errno] || "\u672a\u77e5\u9519\u8bef\uff0cerrno:" + A.errno;
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : _,
							sticky : true,
							closeType : true
						});
					}
				} catch (D) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u670d\u52a1\u5668\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
						sticky : false
					});
				}
			}).error(function () {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u9a8c\u8bc1\u670d\u52a1\u5668\u9519\u8bef:" + textStatus,
					sticky : true,
					closeType : true
				});
			});
		}, function () {
			$("#chunleireader").css("visibility", "visible");
		});
		return;
	},
	downloadCounter : function (C, B) {
		if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
			var F = FileUtils.share_uk,
			D = FileUtils.share_id,
			A = [];
			for (var _ in C) {
				if (C.hasOwnProperty(_)) {
					var E = {
						fid : C[_].fs_id,
						category : C[_].category
					};
					A.push(E);
				}
			}
			$.post(disk.api.RestAPI.MIS_COUNTER, {
				uk : F,
				filelist : $.stringify(A),
				sid : D,
				ctime : FileUtils.share_ctime,
				"public" : FileUtils.share_public_type
			});
			B && $.get(disk.api.RestAPI.SHARE_COUNTER, {
				type : 1,
				shareid : D,
				uk : F,
				sign : FileUtils.share_sign,
				timestamp : FileUtils.share_timestamp,
				t : new Date().getTime(),
				_ : Math.random()
			});
		}
	},
	cancelShare : function (C) {
		var B = disk.Context.getService(disk.Context.SERVICE_DIALOG),
		F = [],
		D,
		J = this,
		G = 0,
		E = 0,
		I = disk.ui.ShareDialog,
		D = "\u5206\u4eab";
		for (var A = 0, _ = C.length; A < _; A++) {
			var H = C[A];
			F.push(H.shareId);
			if (H.status == 1 || H.status == 3 || (!H.fsIds) || H.fsIds.length == 0) {
				continue;
			}
			if (H.public == FileUtils.SHARE_PUBLIC_LINK) {
				G++;
			} else {
				E++;
			}
		}
		if (C.length > 0) {
			B.setVisible(true);
			B.setMessage("\u786e\u8ba4\u53d6\u6d88" + D, "\u53d6\u6d88\u5206\u4eab\u540e\uff0c\u8be5\u6761\u5206\u4eab\u8bb0\u5f55\u5c06\u88ab\u5220\u9664\uff0c\u597d\u53cb\u5c06\u65e0\u6cd5\u518d\u8bbf\u95ee\u6b64\u5206\u4eab\u94fe\u63a5\u3002<br/>\u4f60\u786e\u8ba4\u8981\u53d6\u6d88\u5206\u4eab\u5417\uff1f", function () {
				this.setVisible(false);
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u53d6\u6d88" + D + "\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
				$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.CANCEL_SHARE, FileUtils.bdstoken), {
					shareid_list : $.stringify(F)
				}, function (F) {
					try {
						if (disk.DEBUG) {
							console.log("cancel share response:", F);
						}
						var C = $.parseJSON(F);
						if (C.errno == 0) {
							if (J._mViewMode == FileUtils.VIEW_LIST) {
								J._mInfiniteListView.updateBackedData(J._mLocalCache.obtain(J.resolveCurrentDirKey()));
							} else {
								var _ = J.setGroupData(J._mLocalCache.obtain(J.resolveCurrentDirKey()), J._mGridStep);
								if (J._mInfiniteGridView != null) {
									J._mInfiniteGridView.updateBackedData(_);
								}
							}
							var A = disk.Context.getService(disk.Context.SERVICE_TOAST);
							A.setVisible(false);
							FileUtils._shareDropdownDrawerTrigger(null, true);
							FileUtils.proxyReloadDir();
							A.pending(function () {
								Utilities.useToast({
									toastMode : disk.ui.Toast.MODE_SUCCESS,
									msg : "\u53d6\u6d88\u5916\u94fe" + D + "\u6210\u529f",
									sticky : false
								});
							});
							$("em.publiccnt").html($("em.publiccnt").html() - G);
							$("em.secretcnt").html($("em.secretcnt").html() - E);
							J.cancelQuestionnaire();
						} else {
							var B = disk.util.shareErrorMessage[C.errno] || "\u672a\u77e5\u9519\u8bef\uff0cerrno:" + C.errno;
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : B,
								sticky : true,
								closeType : true
							});
						}
						Page.obtain().resolveToolbarMenu();
					} catch (H) {
						if (disk.DEBUG) {
							console.log("[LOG]=====>cancel public failure msg:", H.message);
						}
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u53d6\u6d88\u5916\u94fe" + D + "\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
							sticky : true,
							closeType : true
						});
					}
				});
			});
		}
	},
	proxyReloadDir : function () {
		FileUtils.getLocalCache().removeAll();
		FileUtils.getLocalCache().removeCategorys();
		FileUtils.issueProxyLoading(true);
		FileUtils.loadDir(FileUtils.resolveCurrentDirKey());
	},
	getListView : function () {
		return this._mViewMode == this.VIEW_LIST ? this._mInfiniteListView : this._mInfiniteGridView;
	},
	repaintListView : function () {
		if (this._mViewMode == this.VIEW_LIST) {
			this._mInfiniteListView.requestLayout();
		} else {
			this._mInfiniteGridView.requestLayout();
		}
	},
	disableSearchAndMediaChoosingFeature : function (D) {
		var B = [document.getElementById("tab-home"), document.getElementById("tab-share"), document.getElementById("tab-public"), document.getElementById("tab-recyle")],
		C = null;
		for (var A = 0, _ = B.length; A < _; A++) {
			C = B[A];
			if (D) {
				if ($(C).hasClass("on")) {
					C.setAttribute("_on", "1");
					$(C).removeClass("on");
					break;
				}
			} else {
				if (C.getAttribute("_on") == "1") {
					$(C).addClass("on");
					break;
				}
			}
		}
		if (D) {
			$("#search").css("display", "none");
			$("#module").css("display", "none");
		} else {
			$("#search").css("display", "block");
			$("#module").css("display", "block");
		}
	},
	hideHisTrigger : function (A) {
		if (A) {
			var _ = document.getElementById("hisTrigger");
			if (_ && $("#hisTrigger").hasClass("aside-history-on")) {
				_.setAttribute("_hide", "1");
				$("#hisTrigger").removeClass("aside-history-on");
			}
		} else {
			_ = document.getElementById("hisTrigger");
			if (_ && _.getAttribute("_hide") == "1") {
				_.setAttribute("_hide", "0");
				$("#hisTrigger").addClass("aside-history-on");
			}
		}
	},
	deleteFiles : function (E, G) {
		var A = disk.Context.getService(disk.Context.SERVICE_DIALOG),
		H = [],
		C = this,
		J = null,
		B = null,
		I = null,
		K = false;
		for (var F = 0, _ = E.length; F < _; F++) {
			J = FileUtils.parseDirPath(E[F].path);
			if (FileUtils.ifSpecialFolder(J)) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u9884\u7f6e\u6587\u4ef6\u5939\u4e0d\u53ef\u5220\u9664\u3002",
					sticky : false
				});
				return;
			}
			if (!K && FileUtils.isInParticularDir(J)) {
				if (J.indexOf(FileUtils.PARTICULAR_DIR_MAP["/\u6765\u81eaPC\u7684\u5907\u4efd\u6587\u4ef6"]) != -1) {
					K = false;
				} else {
					K = true;
				}
			}
			H.push(J);
		}
		B = "\u4f60\u9009\u62e9\u7684\u6587\u4ef6\u5305\u542b\u7b2c\u4e09\u65b9\u5e94\u7528\u4e2d\u7684\u6587\u4ef6";
		if (J.indexOf("/apps/\u8054\u901a\u51b2\u5370") != -1) {
			I = "\u5220\u9664\u540e\u5c06\u5bfc\u81f4\u8054\u901a\u51b2\u5370\u670d\u52a1\u8bfb\u53d6\u6570\u636e\u5f02\u5e38";
		} else {
			if (J.indexOf("/apps/album") != -1) {
				B = "\u4f60\u9009\u62e9\u7684\u6587\u4ef6\u5305\u542b\u767e\u5ea6\u76f8\u518c\u4e2d\u7684\u6587\u4ef6";
				I = "\u5220\u9664\u540e\u767e\u5ea6\u76f8\u518c\u4e2d\u7684\u6587\u4ef6\u4e5f\u5c06\u540c\u6b65\u5220\u9664";
			} else {
				if (J.indexOf("/\u6211\u7684\u5206\u4eab") != -1) {
					B = "\u8be5\u6587\u4ef6\u5df2\u7ecf\u5206\u4eab\u7ed9\u4f60\u7684\u670b\u53cb\u4eec\u4e86";
					I = "\u5220\u9664\u540e\u5c06\u65e0\u6cd5\u518d\u88ab\u67e5\u770b";
				} else {
					I = "\u5220\u9664\u540e\u8be5\u5e94\u7528\u4e2d\u7684\u6587\u4ef6\u4e5f\u5c06\u540c\u6b65\u88ab\u5220\u9664";
				}
			}
		}
		var D = K ? B + "\uff0c" + I + "<br />\u662f\u5426\u7ee7\u7eed\uff1f(\u5220\u9664\u7684\u6587\u4ef6\u53ef\u4ee5\u5728\u56de\u6536\u7ad9\u8fd8\u539f)" : "\u786e\u8ba4\u8981\u628a\u6240\u9009\u6587\u4ef6\u653e\u5165\u56de\u6536\u7ad9\u5417\uff1f<br />(\u5220\u9664\u7684\u6587\u4ef6\u53ef\u4ee5\u5728\u56de\u6536\u7ad9\u8fd8\u539f)";
		if (E.length > 0) {
			A.setVisible(true);
			if ($(".img-dialog").length > 0 && $(".img-dialog").css("display") != "none") {
				$(".b-canvas").css("z-index", "991");
			}
			A.setMessage("\u786e\u8ba4\u5220\u9664", D, function () {
				A.setVisible(false);
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u5220\u9664\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
				$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.FILE_MANAGER + "&opera=delete", FileUtils.bdstoken), {
					filelist : $.stringify(H)
				}, function (B) {
					var A = null;
					try {
						A = $.parseJSON(B);
					} catch (J) {
						if (disk.DEBUG) {
							console.log("delete falure msg:", J.message);
						}
					}
					if (A != null && A.errno == 0) {
						var _ = disk.Context.getService(disk.Context.SERVICE_TOAST);
						_.setVisible(false);
						var F = -1,
						I = null;
						if (FileUtils.getModule() == "category") {
							F = disk.util.FileCache.KEY_MODE_CATEGORY;
							I = FileUtils.getType();
						} else {
							if (FileUtils.inSearchMode()) {
								F = disk.util.FileCache.KEY_MODE_SEARCH;
								I = FileUtils._mLastQueryTerms;
							} else {
								F = disk.util.FileCache.KEY_MODE_NORMAL;
								I = FileUtils.resolveCurrentDirKey();
							}
						}
						var E = FileUtils.getLocalCache().deleteByMode(F, I, H);
						if (E) {
							FileUtils.clearAllDocData();
							var D = FileUtils.getListView();
							D.setItemsChecked(false);
							B = FileUtils.getLocalCache().obtainByMode(F, I);
							if (C._mViewMode != FileUtils.VIEW_LIST) {
								B = C.setGroupData(B, C._mGridStep);
							}
							D.updateBackedData(B);
							Page.obtain().resolveToolbarMenu();
							if (typeof G == "function") {
								G();
							}
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_SUCCESS,
								msg : '文件已删除到"回收站"',
								sticky : false
							});
							return;
						}
					}
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "删除文件失败，请稍候重试",
						sticky : true,
						closeType : true
					});
				});
				if ($(".img-dialog").length > 0 && $(".img-dialog").css("display") != "none") {
					$(".b-canvas").css("z-index", "890");
				}
			}, function () {
				if ($(".img-dialog").length > 0 && $(".img-dialog").css("display") != "none") {
					$(".b-canvas").css("z-index", "890");
				}
			});
		}
	},
	transferFiles : function (K, E, B, L) {
		var G = this,
		A = disk.ui.LoginDialog.obtain(),
		M = [],
		P = FileUtils.sysUK || B,
		_ = Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_LOADING,
				msg : "\u6b63\u5728\u4fdd\u5b58\u6587\u4ef6...",
				sticky : true
			});
		if (!E) {
			E = FileUtils.share_id;
		}
		var N = K,
		H = null,
		F = 1,
		J = function (A) {
			_ = Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u52a0\u8f7d\u6570\u636e",
					sticky : true
				});
			H = A;
			if (!H || H.length <= 0) {
				if (disk.DEBUG) {
					console.log("transfer more files is empty");
				}
				_ = Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u60a8\u8fd8\u6ca1\u6709\u9009\u62e9\u4fdd\u5b58\u7684\u6587\u4ef6",
						sticky : false
					});
				return;
			}
			if (FileUtils.loginState != "1") {
				_.setVisible(false);
				disk.PassportInit.netdiskLogin({
					reload : true
				});
			} else {
				if (!G._mMoveSaveDialog) {
					G._mMoveSaveDialog = new disk.ui.MoveSaveDialog();
				}
				G._mMoveSaveDialog.onConsent = function (I) {
					if (disk.DEBUG) {
						console.log("move save more files to path:" + I);
					}
					var C = I,
					D = [],
					A = E,
					J = Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_LOADING,
							position : disk.ui.Panel.TOP,
							msg : "\u6b63\u5728\u4fdd\u5b58\uff0c\u8bf7\u7a0d\u5019&hellip;",
							sticky : true
						});
					if (typeof B != "undefined") {
						J.setGravity(disk.ui.Panel.CENTER);
					}
					if (A.length > 0) {
						D.push("&shareid=");
						D.push(A);
					}
					$.ajax({
						type : "post",
						url : disk.api.RestAPI.normalize(disk.api.RestAPI.TRANSFER + "&ondup=newcopy&async=" + F + "&from=" + encodeURIComponent(P) + D.join(""), FileUtils.bdstoken),
						data : {
							path : I,
							filelist : $.stringify(H)
						},
						timeout : 100000,
						success : function (O) {
							try {
								var P = $.parseJSON(O);
							} catch (T) {
								if (disk.DEBUG) {
									console.log("[LOG]====>parse json error on show property dialog ", T.message);
								}
								Utilities.useToast({
									toastMode : disk.ui.Toast.MODE_CAUTION,
									position : disk.ui.Panel.TOP,
									msg : "\u6570\u636e\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
									sticky : false
								});
							}
							if (P == null) {
								Utilities.useToast({
									toastMode : disk.ui.Toast.MODE_CAUTION,
									position : disk.ui.Panel.TOP,
									msg : "\u7f51\u7edc\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
									sticky : false
								});
							} else {
								if (P.errno == -6) {
									_.setVisible(false);
									G._mMoveSaveDialog.setVisible(false);
									disk.PassportInit.netdiskLogin({
										reload : true
									});
								} else {
									if (P.errno == 0) {
										var R = FileUtils.share_uk,
										A = FileUtils.share_id,
										B = P.task_id,
										I = [];
										for (var J in N) {
											var K = {
												fid : N[J].fs_id,
												category : N[J].category
											};
											I.push(K);
										}
										if (B > 0) {
											var H,
											Q = setInterval(function () {
													$.get("/share/taskquery?taskid=" + B, function (_) {
														_ = $.parseJSON(_);
														if (!_) {
															Utilities.useToast({
																toastMode : disk.ui.Toast.MODE_CAUTION,
																position : disk.ui.Panel.TOP,
																msg : "\u7f51\u7edc\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
																sticky : true,
																closeType : true
															});
															clearInterval(Q);
															return;
														}
														if (_.errno == 0) {
															if (_.task_errno != 0) {
																var A = "";
																if (_.task_errno == -30) {
																	A = "\u4fdd\u5b58\u7684\u6587\u4ef6\u90e8\u5206\u5df2\u5b58\u5728\uff0c<a style='color:blue;text-decoration:underline' href='http://pan.baidu.com/disk/home#dir/path=" + encodeURIComponent(C || "/") + "'>\u70b9\u51fb\u67e5\u770b</a>\u5df2\u5b58\u5728\u7684\u6587\u4ef6";
																} else {
																	A = disk.util.shareErrorMessage[_.task_errno] || "\u6587\u4ef6\u4fdd\u5b58\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5";
																}
																Utilities.useToast({
																	toastMode : disk.ui.Toast.MODE_CAUTION,
																	msg : A,
																	sticky : true,
																	position : disk.ui.Panel.TOP,
																	closeType : true
																});
																clearInterval(Q);
															} else {
																if (_.status == "pending") {
																	Utilities.useToast({
																		toastMode : disk.ui.Toast.MODE_LOADING,
																		msg : "\u6b63\u5728\u51c6\u5907\u4fdd\u5b58\uff0c\u8bf7\u7a0d\u5019&hellip;",
																		sticky : true,
																		position : disk.ui.Panel.TOP,
																		closeType : false
																	});
																} else {
																	if (_.status == "running") {
																		var B = 0;
																		if (typeof H != "undefined" && (!_.progress)) {
																			B = H;
																		} else {
																			if (!_.progress) {
																				B = 0;
																			} else {
																				B = Math.min(Math.max(parseInt(_.progress, 10), 0), 100);
																			}
																			H = B;
																		}
																		Utilities.useToast({
																			toastMode : disk.ui.Toast.MODE_LOADING,
																			msg : "\u6b63\u5728\u4fdd\u5b58\uff0c\u8bf7\u7a0d\u5019&hellip;&nbsp;&nbsp;&nbsp;\u5df2\u5b8c\u6210\uff1a" + B + "%",
																			sticky : true,
																			position : disk.ui.Panel.TOP,
																			closeType : false
																		});
																	} else {
																		if (_.status == "success") {
																			Utilities.useToast({
																				toastMode : disk.ui.Toast.MODE_SUCCESS,
																				msg : "\u4fdd\u5b58\u6210\u529f\uff0c<a style='color:blue;text-decoration:underline' href='http://pan.baidu.com/disk/home#dir/path=" + encodeURIComponent(C || "/") + "'>\u70b9\u51fb\u67e5\u770b</a>\u4fdd\u5b58\u7684\u6587\u4ef6",
																				sticky : true,
																				position : disk.ui.Panel.TOP,
																				closeType : true
																			});
																			clearInterval(Q);
																		} else {
																			Utilities.useToast({
																				toastMode : disk.ui.Toast.MODE_CAUTION,
																				msg : "\u6587\u4ef6\u4fdd\u5b58\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
																				sticky : true,
																				position : disk.ui.Panel.TOP,
																				closeType : true
																			});
																			clearInterval(Q);
																		}
																	}
																}
															}
														} else {
															Utilities.useToast({
																toastMode : disk.ui.Toast.MODE_CAUTION,
																msg : disk.util.shareErrorMessage[_.errno] || "\u6587\u4ef6\u4fdd\u5b58\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
																sticky : true,
																position : disk.ui.Panel.TOP,
																closeType : true
															});
															clearInterval(Q);
														}
													});
												}, 2500);
											Utilities.useToast({
												toastMode : disk.ui.Toast.MODE_LOADING,
												msg : "\u7cfb\u7edf\u6b63\u5728\u4e3a\u4f60\u4fdd\u5b58\uff0c\u8bf7\u7a0d\u5019&hellip;",
												position : disk.ui.Panel.TOP,
												sticky : true
											});
										} else {
											if (P.errno == 111) {
												Utilities.useToast({
													toastMode : disk.ui.Toast.MODE_CAUTION,
													msg : "\u5f53\u524d\u6709\u4e00\u4e2a\u4fdd\u5b58\u4efb\u52a1\u6b63\u5728\u8fdb\u884c\uff0c\u8bf7\u7a0d\u5019\u4fdd\u5b58",
													sticky : false,
													position : disk.ui.Panel.TOP,
													closeType : false
												});
											} else {
												Utilities.useToast({
													toastMode : disk.ui.Toast.MODE_SUCCESS,
													msg : "\u4fdd\u5b58\u6210\u529f\uff0c<a style='color:blue;text-decoration:underline' href='http://pan.baidu.com/disk/home#dir/path=" + encodeURIComponent(C || "/") + "'>\u70b9\u51fb\u67e5\u770b</a>\u4fdd\u5b58\u7684\u6587\u4ef6",
													sticky : true,
													position : disk.ui.Panel.TOP,
													closeType : true
												});
											}
										}
										$.post(disk.api.RestAPI.MIS_COUNTER, {
											uk : R,
											filelist : $.stringify(I),
											sid : A,
											t : new Date().getTime(),
											_ : Math.random(),
											ctime : FileUtils.share_ctime,
											"public" : FileUtils.share_public_type
										});
										G._mMoveSaveDialog.refreshTree();
									} else {
										if (P.errno == 111) {
											Utilities.useToast({
												toastMode : disk.ui.Toast.MODE_CAUTION,
												msg : "\u5f53\u524d\u6709\u4e00\u4e2a\u4fdd\u5b58\u4efb\u52a1\u6b63\u5728\u8fdb\u884c\uff0c\u8bf7\u7a0d\u5019\u4fdd\u5b58",
												sticky : false,
												position : disk.ui.Panel.TOP,
												closeType : false
											});
										} else {
											if (P.errno == 12) {
												var F = P.info,
												J = 0,
												M = F.length,
												D,
												E;
												for (; J < M; J++) {
													if (F[J].errno == 0) {
														continue;
													}
													var T = F[J],
													S = F[J].path,
													E = S.slice(S.lastIndexOf("/") + 1);
													if (F[J].errno == -33) {
														D = "一次支持操作" + (P.limit || 2000) + "个，减点试试吧";
													} else {
														D = disk.util.shareErrorMessage[F[J].errno];
													}
													D = D || "\u8bf7\u7a0d\u5019\u91cd\u8bd5";
													switch (F[J].errno) {
													case  - 30:
														D += ' <a style="color:blue;text-decoration:underline" href="http://pan.baidu.com/disk/home#dir/path=' + S + '">\u70b9\u51fb\u67e5\u770b</a>';
														break;
													default:
														break;
													}
													break;
												}
												Utilities.useToast({
													toastMode : disk.ui.Toast.MODE_CAUTION,
													position : disk.ui.Panel.TOP,
													msg : "\u4fdd\u5b58\u5931\u8d25\uff0c" + D,
													sticky : true,
													closeType : true
												});
											} else {
												if (P.errno == 5) {
													Utilities.useToast({
														toastMode : disk.ui.Toast.MODE_CAUTION,
														msg : "\u6587\u4ef6\u5df2\u5b58\u5728\uff0c\u4e0d\u7528\u8f6c\u7ed9\u81ea\u5df1\u5566",
														sticky : true,
														closeType : true
													});
												} else {
													if (P.errno == -33) {
														Utilities.useToast({
															toastMode : disk.ui.Toast.MODE_CAUTION,
															msg : "\u4e00\u6b21\u652f\u6301\u64cd\u4f5c" + (P.limit || 2000) + "\u4e2a\uff0c\u51cf\u70b9\u8bd5\u8bd5\u5427",
															sticky : true,
															closeType : true
														});
													} else {
														Utilities.useToast({
															toastMode : disk.ui.Toast.MODE_CAUTION,
															msg : "\u4fdd\u5b58\u5931\u8d25\uff0c" + (disk.util.shareErrorMessage[P.errno] || "\u8bf7\u7a0d\u5019\u91cd\u8bd5"),
															sticky : true,
															closeType : true
														});
													}
												}
											}
										}
									}
								}
							}
							if (typeof L == "function") {
								L(P);
							}
						}
					});
				};
				G._mMoveSaveDialog.setVisible(true);
				FileUtils.sendUserReport("share_transfer");
				if (FileUtils._mUserInfo) {
					FileUtils._mDiskLog.send({
						type : "login_transfer_activate"
					});
				}
				_.setVisible(false);
			}
		};
		for (var I = 0, O = K.length; I < O; I++) {
			var D = K[I],
			C = FileUtils.parseDirPath(D.path);
			M.push(C);
		}
		J(M);
	},
	toFriendlyFileSize : function (_) {
		if (_ < 1024) {
			return Math.round(_) + "B";
		} else {
			if (_ < 1024 * 1024 && _ >= 1024) {
				return Math.round(_ / 1024) + "KB";
			} else {
				if (_ < 1024 * 1024 * 1024 && _ >= 1024 * 1024) {
					return (_ / 1024 / 1024).toFixed(1) * 10 / 10 + "M";
				} else {
					return (_ / 1024 / 1024 / 1024).toFixed(2) * 100 / 100 + "G";
				}
			}
		}
	},
	switchToGrid : function () {
		if (this._mInfiniteListView) {
			this._mInfiniteListView.awakeScrollbar(false);
		}
		if (this._mInfiniteListView != null && !this._mInfiniteListView.locked()) {
			this._mInfiniteListView.lock(true, true);
		}
		$("#infiniteListView").css("display", "none");
		$("#infiniteGridView").css("display", "block");
		this.load(this.resolveCurrentDirKey(), true);
		$("#barCmdViewList").removeClass("select");
	},
	getViewMode : function () {
		return this._mViewMode;
	},
	switchViewMode : function (A, _) {
		this._mLastViewMode = this._mViewMode;
		this._mViewMode = A;
		switch (A) {
		case FileUtils.VIEW_GRID_ICON:
		case FileUtils.VIEW_GRID_THUMB:
			FileUtils._mGridStep = Math.floor($("#header-shaw").outerWidth() / 150) || 2;
			$("#barCmdViewSmall").addClass("select");
			FileUtils.switchToGrid();
			break;
		case FileUtils.VIEW_LIST:
		default:
			if (FileUtils._mInfiniteGridView) {
				this._mInfiniteGridView.awakeScrollbar(false);
				if (!this._mInfiniteGridView.locked()) {
					this._mInfiniteGridView.lock(true, true);
				}
			}
			$("#infiniteGridView").css("display", "none");
			$("#infiniteListView").css("display", "block");
			if (FileUtils._mInfiniteListView) {
				FileUtils._mInfiniteListView.lock(false, true);
				FileUtils._mInfiniteListView.awakeScrollbar(true);
			}
			this.load(this.resolveCurrentDirKey(), true);
			$("#barCmdViewList").addClass("select");
			$("#barCmdViewSmall").removeClass("select");
			break;
		}
		if (typeof this._mViewModeSwitchWatcher == "function") {
			this._mViewModeSwitchWatcher.call(this, A == FileUtils.VIEW_GRID_ICON || A == FileUtils.VIEW_GRID_THUMB);
		}
	},
	_listenTransferTriggers : function (_) {
		var A = this;
		$(_).click(function (C) {
			if (A.isListViewLocked()) {
				return;
			}
			var _ = this.parentNode.parentNode.parentNode.getAttribute("_position");
			if (typeof _ != "undefined") {
				_ = parseInt(_);
				var B = FileUtils.getElementData(_);
				FileUtils.transferFiles([B]);
			}
			C.preventDefault();
			C.stopPropagation();
			return false;
		});
	},
	_listenMoreTriggers : function (_) {
		var A = this;
		$(_).click(function (A) {
			$(this.parentNode).addClass("dd-hover");
			this.parentNode.parentNode.parentNode.style.zIndex = "3";
			var _ = MoreOptionsDropDown.obtain().attach(this);
			A.preventDefault();
			A.stopPropagation();
			return false;
		});
		$(_.parentNode).hover(function () {}, function () {
			this.parentNode.parentNode.style.zIndex = "1";
			$(this).removeClass("dd-hover");
		});
	},
	_listenMouseRightMenuTriggers : function (_) {
		var A = this;
		_.oncontextmenu = function (F) {
			var F = F || window.event || arguments[0],
			E = $(".input-cbx", this),
			D = this.getAttribute("_position");
			if (D != null) {
				D = parseInt(D);
			}
			if (A.isListViewLocked() || F.shiftKey) {
				return;
			}
			if (!F.ctrlKey) {
				switch (FileUtils._mViewMode) {
				case FileUtils.VIEW_GRID_ICON:
				case FileUtils.VIEW_GRID_THUMB:
					if (!A._mInfiniteGridView.isItemChecked(D)) {
						$(".icon-item").removeClass("on");
						A._mInfiniteGridView.setItemsChecked(false);
						A._mInfiniteGridView.setItemChecked(D, true);
						$(this).addClass("on");
						E.addClass("input-cbx-on");
					}
					break;
				case FileUtils.VIEW_LIST:
				default:
					if (!A._mInfiniteListView.isItemChecked(D)) {
						$(".file-item").removeClass("on");
						A._mInfiniteListView.setItemsChecked(false);
						$(".input-cbx").removeClass("input-cbx-on");
						A._mInfiniteListView.setItemChecked(D, true);
						$(this).addClass("on");
						E.addClass("input-cbx-on");
					}
					break;
				}
			}
			if (F.ctrlKey) {
				if (D != null) {
					$(this).addClass("on");
					E.addClass("input-cbx-on");
					if (A._mInfiniteListView != null) {
						A._mInfiniteListView.setItemChecked(D, true);
					}
					if (A._mInfiniteGridView != null) {
						A._mInfiniteGridView.setItemChecked(D, true);
					}
				}
			}
			FileUtils.repaintListView();
			Page.obtain().resolveToolbarMenu();
			var B = 0,
			C = 0;
			if (F.pageX || F.pageY) {
				B = F.pageY - 1,
				C = F.pageX - 1;
			} else {
				C = F.clientX + document.body.scrollLeft - document.body.clientLeft;
				B = F.clientY + document.body.scrollTop - document.body.clientTop;
			}
			if (FileUtils.getElementData(D).status == 0) {
				FileUtils._shareDropdownDrawerTrigger(this);
			}
			var _ = $(this).get(0);
			if (!Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				A._resolveRightContextMenu(_, B, C);
				if (F.preventDefault) {
					F.preventDefault();
				}
				if (F.stopPropagation) {
					F.stopPropagation();
				}
			} else {
				return true;
			}
			return false;
		};
	},
	_resolveRightContextMenu : function (C, B, E) {
		var F = MoreOptionsDropDown.obtain().attach(C, B, E),
		D = F._mUI.pane,
		A = false;
		$(D).find("li").removeClass("menu-separator b-bdr-7");
		$(D).find("li").each(function (B, _) {
			if (B % 2 == 0 && B > 0) {
				A = true;
			}
			if (A === true && this.style.display !== "none") {
				$(this).addClass("menu-separator b-bdr-7");
				A = false;
			}
		});
		var _ = this.getListViewCheckedItems();
		$(D).show();
		$(D).find("li a").css({
			"padding-left" : "17px",
			"text-align" : "left",
			"line-height" : "22px"
		});
		$("body").bind("mousedown", this._hideRightMouseMenu);
	},
	_hideRightMouseMenu : function () {
		$("#right-context-menu").hide();
		$("body").unbind("mousedown", this._hideRightMouseMenu);
	},
	_listenRenameTriggers : function (_) {
		var A = this;
		$(_).click(function (E) {
			if (A.isListViewLocked()) {
				return false;
			}
			var _ = this.parentNode.parentNode.parentNode,
			B = _.getAttribute("_position");
			if (typeof B != "undefined") {
				B = parseInt(B);
				var C = FileUtils.getElementData(B);
				if (C) {
					var D = FileUtils.parseDirPath(C.path);
					if (FileUtils.ifSpecialFolder(D)) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u9884\u7f6e\u6587\u4ef6\u5939\u4e0d\u53ef\u91cd\u547d\u540d",
							sticky : false
						});
					} else {
						FileUtils.prepareFolderBuilder(FileUtils.UPDATE, C, _, true);
					}
				}
			}
			if (E.preventDefault) {
				E.preventDefault();
				E.stopPropagation();
			} else {
				E.cancelBubble = true;
				E.returnValue = false;
			}
			return false;
		});
	},
	_listenShareLinkTriggers : function (_) {
		var A = this;
		$(_).click(function (C) {
			if (A.isListViewLocked()) {
				return;
			}
			var _ = this.parentNode.parentNode.parentNode.getAttribute("_position");
			if (typeof _ != "undefined") {
				_ = parseInt(_);
				var B = FileUtils.getElementData(_);
				if (B) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_LOADING,
						msg : "\u6b63\u5728\u83b7\u53d6\u5730\u5740\uff0c\u8bf7\u7a0d\u5019&hellip;",
						sticky : true
					});
					$.post(disk.api.RestAPI.SINGLE_PUBLIC_URI, {
						path : FileUtils.parseDirPath(B.path)
					}, function (B) {
						var _ = null;
						try {
							_ = $.parseJSON(B);
						} catch (D) {
							if (disk.DEBUG) {
								console.log("[LOG]====>parse json error on get public uri", D.message);
							}
						}
						if (_ == null || _.errno != 0) {
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : "\u7f51\u7edc\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
								sticky : false
							});
						} else {
							disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
							var C = _.url;
							if (A._mSingleURLDialog == null) {
								A._mSingleURLDialog = new disk.ui.SingleURLDialog();
							}
							A._mSingleURLDialog.setURL(C);
							A._mSingleURLDialog.setVisible(true);
						}
					});
				}
			}
			if (C.preventDefault) {
				C.preventDefault();
				C.stopPropagation();
			} else {
				C.cancelBubble = true;
				C.returnValue = false;
			}
		});
	},
	performCopyShareLink : function (_) {
		var C = this;
		if (C.isListViewLocked()) {
			return;
		}
		var A = _.getAttribute("_position");
		if (typeof A != "undefined") {
			A = parseInt(A);
			var B = FileUtils.getElementData(A);
			if (B) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
				$.post(disk.api.RestAPI.SINGLE_PUBLIC_URI, {
					path : FileUtils.parseDirPath(B.path)
				}, function (A) {
					var _ = null;
					try {
						_ = $.parseJSON(A);
					} catch (D) {
						if (disk.DEBUG) {
							console.log("[LOG]====>parse json error on get public uri", D.message);
						}
					}
					if (_ == null || _.errno != 0) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u7f51\u7edc\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
							sticky : false
						});
					} else {
						disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
						var B = _.url;
						if (C._mSingleURLDialog == null) {
							C._mSingleURLDialog = new disk.ui.SingleURLDialog();
						}
						C._mSingleURLDialog.setURL(B);
						C._mSingleURLDialog.setVisible(true);
					}
				});
			}
		}
		return false;
	},
	_listenCancelShareTriggers : function (A, _) {
		var B = this;
		$(A).click(function (D) {
			if (B.isListViewLocked()) {
				return;
			}
			var A = this.parentNode.parentNode.parentNode.getAttribute("_position");
			if (typeof A != "undefined") {
				A = parseInt(A);
				var C = FileUtils.getElementData(A);
				if (C) {
					FileUtils.cancelShare([C], _);
				}
			}
			if (D.preventDefault) {
				D.preventDefault();
				D.stopPropagation();
			} else {
				D.cancelBubble = true;
				D.returnValue = false;
			}
		});
	},
	_listenCheckboxDoubleClickTriggers : function (_) {
		$(_).dblclick(function (_) {
			if (_.preventDefault) {
				_.preventDefault();
			}
			if (_.stopPropagation) {
				_.stopPropagation();
			}
			return false;
		});
	},
	_listenCheckboxTriggers : function (_) {
		var A = this;
		_.onclick = function (I) {
			var I = I || window.event || agruments[0],
			E = this.getAttribute("_position"),
			E = parseInt(E),
			F;
			if (FileUtils.isListViewLocked()) {
				return false;
			}
			var _ = (FileUtils._mViewMode == FileUtils.VIEW_LIST) ? FileUtils._mInfiniteListView : FileUtils._mInfiniteGridView;
			if (!I.shiftKey) {
				A._lastclick = this.getAttribute("_position");
				F = !_.isItemChecked(E);
				if ($(".files-header").hasClass("on")) {
					$(".files-header").removeClass("on");
				}
				if (E != null) {
					_.setItemChecked(E, F);
				}
				A._lastShiftClick = null;
			} else {
				var G = this.getAttribute("_position"),
				D = Math.min(G, A._lastclick),
				H = Math.max(G, A._lastclick);
				if (A._lastShiftClick) {
					var C = Math.min(A._lastclick, A._lastShiftClick),
					B = Math.max(A._lastclick, A._lastShiftClick);
					while (C <= B) {
						_.setItemChecked(C, false);
						C++;
					}
				} else {}

				while (D <= H) {
					_.setItemChecked(D, true);
					D++;
				}
				A._lastShiftClick = this.getAttribute("_position");
			}
			FileUtils.repaintListView();
			A.changeProperty();
			Page.obtain().resolveToolbarMenu();
			if (I.preventDefault) {
				I.preventDefault();
				I.stopPropagation();
			} else {
				I.cancelBubble = true;
				I.returnValue = false;
			}
		};
	},
	_listenDragFileTriggers : function (_) {
		var A = this;
		if (Page.inViewMode(Page.VIEW_HOME)) {
			$(_).mousedown(function (C) {
				if (A.isListViewLocked() || $(this).hasClass("on") === false || A._mSelectionArbitrate.hasClass("on") || C.which !== 1) {
					return;
				}
				var _ = (FileUtils._mViewMode == FileUtils.VIEW_LIST) ? FileUtils._mInfiniteListView : FileUtils._mInfiniteGridView,
				B = _.getElementsData()[this.getAttribute("_position")];
				A.triggerDragFile(true);
				C.preventDefault();
			}).mouseup(function (F) {
				if (A._mDraging !== true) {
					return;
				}
				var _ = (FileUtils._mViewMode == FileUtils.VIEW_LIST) ? FileUtils._mInfiniteListView : FileUtils._mInfiniteGridView,
				D = _.getElementsData()[this.getAttribute("_position")];
				if (D.isdir === 0 || $(this).hasClass("on") === true) {
					A.triggerDragFile(false);
					return;
				}
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u79fb\u52a8\u6587\u4ef6\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
				var E = FileUtils.parseDirPath(D.path),
				B = FileUtils.getListViewCheckedItems(),
				C = E == disk.util.DirectoryLocation.DIR_ROOT ? "/" : E;
				FileUtils.sendMoveCopyFileMessage(disk.ui.MoveCopyDialog.MOVE, B, C, function (H) {
					var G = $.parseJSON(H);
					if (G.errno == 0) {
						if (disk.DEBUG) {
							console.log("move files success, prepare to update local cache");
						}
						for (var D = 0, C = B.length; D < C; D++) {
							A._mLocalCache.preseveAndDescendentlyRemove(A.resolveCurrentDirKey(), FileUtils.parseDirPath(B[D].path));
						}
						if (A._mViewMode == FileUtils.VIEW_LIST) {
							A._mInfiniteListView.updateBackedData(A._mLocalCache.obtain(A.resolveCurrentDirKey()));
						} else {
							var _ = A.setGroupData(A._mLocalCache.obtain(A.resolveCurrentDirKey()), A._mGridStep);
							if (A._mInfiniteGridView != null) {
								A._mInfiniteGridView.updateBackedData(_);
							}
						}
						FileUtils.getLocalCache().removeCategorys();
						FileUtils.resolveParentDirEmptyState(E);
						FileUtils.getLocalCache().remove(FileUtils.resolveDirKey(E));
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_SUCCESS,
							msg : "移动文件成功。",
							sticky : false
						});
						return;
					} else {
						if (G.errno == 12) {
							Page.obtain().dealCopyMoveBatchCallback(G, disk.ui.MoveCopyDialog.MOVE);
							FileUtils.loadDir(FileUtils.resolveCurrentDirKey());
							return;
						} else {
							var F = disk.util.ErrorMessage[G.errno] || "未知错误" + G.errno;
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : F,
								sticky : false
							});
						}
					}
					Page.obtain().resolveToolbarMenu();
					FileUtils.changeProperty();
				});
			});
		}
	},
	_shareDropdownDrawerTrigger : function (D, A, E) {
		if (!Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) && !Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
			return;
		}
		var F = this,
		_ = ".share-warn",
		C = this.getListViewCheckedItems(),
		B = E ? E : 0;
		$(_).hide(E);
		FileUtils._mDropdownDrawerFlag = FileUtils._mDropdownDrawerFlag == -1 ? -1 : 0;
		if (A) {
			$(_).attr("emulate-lost-selection", "1");
			return;
		}
		if (C.length == 1) {
			$(_, D).show(E);
			F._initClipboard(D);
		}
	},
	_checkIsBuyFile : function (_) {
		_ = _ || "";
		return false;
	},
	_initClipboard : function (B) {
		var I = this,
		_ = FileUtils.getListViewCheckedItems()[0],
		G = _.shareId,
		A = _.passwd,
		D = Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) ? FileUtils.sysUK_Master : FileUtils.sysUK,
		E = Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) ? encodeURI(disk.getParam("third")) : FileUtils.third,
		C;
		if (_.shorturl) {
			C = "http://" + location.host + "/s/" + _.shorturl;
		} else {
			C = "http://" + location.host + "/share/link?shareid=" + G + "&uk=" + D;
		}
		C = E == 0 ? C : C + "&third=" + E;
		C = A !== "0" ? "\u94fe\u63a5:" + C + " \u63d0\u53d6\u5bc6\u7801:" + A : C;
		var H = function () {
			var _ = new ZeroClipboard.Client();
			try {
				if (_ != null) {
					I._shareflash = _;
					I._shareflash.setText(C);
					$(".share_copy_btn", B).removeClass("disabled");
					_.setHandCursor(true);
					_.addEventListener("complete", function (_) {
						$(".share_copy_btn", B).removeClass("bbtn-active bbtn-hover");
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_SUCCESS,
							msg : "\u590d\u5236\u5230\u526a\u5207\u677f\u4e86\uff0c\u7c98\u8d34\u7ed9\u60a8\u7684\u670b\u53cb\u5427",
							sticky : false
						});
						I.setVisible(false);
					});
					_.addEventListener("mouseOver", function (_) {
						$(".share_copy_btn", B).addClass("bbtn-hover");
					});
					_.addEventListener("mouseOut", function (_) {
						$(".share_copy_btn", B).removeClass("bbtn-active bbtn-hover");
					});
					_.addEventListener("mouseDown", function (_) {
						$(".share_copy_btn", B).addClass("bbtn-active");
					});
					_.addEventListener("mouseUp", function (_) {
						$(".share_copy_btn", B).removeClass("bbtn-active");
					});
					_.glue("share_copy_id", "share_copyouter_id");
					$(".share_copy_btn", B).next().click(function (_) {
						if (_.preventDefault) {
							_.preventDefault();
							_.stopPropagation();
						} else {
							_.cancelBubble = true;
							_.returnValue = false;
						}
					});
				} else {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u52a0\u8f7d\u590d\u5236\u7ec4\u4ef6\u5931\u8d25\uff0c\u8bf7\u624b\u52a8\u590d\u5236\u8f93\u5165\u6846\u4e2d\u7684\u94fe\u63a5",
						sticky : true,
						closeType : true
					});
				}
			} catch (A) {}

		},
		F = function () {
			I = this;
			$("#share_copy_id").removeAttr("id");
			$(".share_copy_btn", B).addClass("disabled");
			$(".share_copy_btn", B).attr("id", "share_copy_id");
			$(".share_copy_btn", B).removeClass("disabled");
			$(".share_copy_btn", B).click(function (_) {
				window.clipboardData.setData("Text", $(this).attr("_link"));
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_SUCCESS,
					msg : "\u590d\u5236\u5230\u526a\u5207\u677f\u4e86\uff0c\u7c98\u8d34\u7ed9\u60a8\u7684\u670b\u53cb\u5427\u3002",
					sticky : false
				});
				if (_.preventDefault) {
					_.preventDefault();
					_.stopPropagation();
				} else {
					_.cancelBubble = true;
					_.returnValue = false;
				}
			});
		};
		$(".share_copy_btn", B).attr("_link", C);
		if (typeof window.clipboardData != "undefined") {
			if (!$(".share_copy_btn", B).attr("_install")) {
				F();
				$(".share_copy_btn", B).attr("_install", 1);
			}
			FileUtils._mDropdownDrawerFlag = 1;
		} else {
			$("#share_copy_id").length && $("#share_copy_id").parent()[0].removeChild($("#share_copy_id").next()[0]);
			$("#share_copy_id").removeAttr("id");
			$("#share_copyouter_id").removeAttr("id");
			$(".share_copy_btn", B).addClass("disabled");
			$(".share-warn-tit div:first", B).attr("id", "share_copyouter_id");
			$(".share_copy_btn", B).attr("id", "share_copy_id");
			if (this._mDropdownDrawerFlag == -1) {
				$.getScript("/res/static/js/plug/ZeroClipboard.js", function (_) {
					ZeroClipboard.setMoviePath("/res/static/images/ZeroClipboard10.swf");
					H();
				});
			} else {
				H();
			}
			FileUtils._mDropdownDrawerFlag = 1;
			return;
		}
	},
	_listenClickTriggers : function (_) {
		var A = this;
		$(_).click(function (S) {
			var T = $(".input-cbx", this),
			E = parseInt(this.getAttribute("_position")),
			F = S.target || S;
			A._mSelectionArbitrate.removeClass("on");
			var I = $(".share-warn").attr("emulate-lost-selection") == "1";
			$(".share-warn").removeAttr("emulate-lost-selection");
			if (A.isListViewLocked()) {
				return;
			}
			var _ = (FileUtils._mViewMode === FileUtils.VIEW_LIST) ? A._mInfiniteListView : A._mInfiniteGridView;
			if (Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				var L = $(this).hasClass("on") && _.getCheckedItems().length === 1;
				_.setItemsChecked(false);
				$(".file-item").removeClass("on");
				if (L) {
					A._shareDropdownDrawerTrigger(this);
					$(this).removeClass("on");
					return;
				}
				_.setItemChecked(E, true);
				$(this).addClass("on");
			} else {
				if (!S.ctrlKey && !S.shiftKey) {
					L = $(this).hasClass("on") && _.getCheckedItems().length === 1 && !I;
					A._lastclick = this.getAttribute("_position");
					_.setItemsChecked(false);
					switch (FileUtils._mViewMode) {
					case FileUtils.VIEW_GRID_ICON:
					case FileUtils.VIEW_GRID_THUMB:
						$(".icon-item").removeClass("on");
						break;
					case FileUtils.VIEW_LIST:
					default:
						$(".file-item").removeClass("on");
						break;
					}
					if (L) {
						if (F.className.match(/icon/)) {
							A._shareDropdownDrawerTrigger(this);
							Page.obtain().resolveToolbarMenu();
						} else {
							A._shareDropdownDrawerTrigger(this);
							Page.obtain().resolveToolbarMenu();
							return;
						}
					}
					_.setItemChecked(E, true);
					$(this).addClass("on");
					A._lastShiftClick = null;
					if (F.className.match(/icon/)) {
						if (F.className.indexOf("disabled") != -1) {
							return;
						}
						if (F.className == "icon-more-sfile") {
							$(F).next().show();
							Page.obtain().resolveToolbarMenu();
						}
						Page.obtain().resolveToolbarMenu();
						var R = A.getElementData(E),
						O = FileUtils.isTopParticularDir(FileUtils.parseDirPath(R.path || R.typicalPath)),
						C = F.parentNode.getElementsByTagName("div");
						if (C) {
							for (var M = 0, B = C.length; M < B; M++) {
								var K = C[M].className;
								if (K.indexOf("icon-rename") == 0 || K.indexOf("icon-move") == 0 || K.indexOf("icon-delete") == 0) {
									if (O) {
										$(C[M]).addClass("disabled");
									} else {
										$(C[M]).removeClass("disabled");
									}
								}
							}
						}
						var G = F.getAttribute("_i");
						if (G == null && F.className != "icon-more-sfile") {
							throw new Error("Menu Not Registered");
						}
						G = parseInt(G);
						var Q = Page.obtain().dispatchActionPerformed(G, this, S);
						if (!Q) {
							if (disk.DEBUG) {
								console.log("Suppress a download event, hand it over");
							}
							return false;
						}
						return true;
					}
				} else {
					if (!S.shiftKey) {
						A._lastclick = this.getAttribute("_position");
						_.setItemChecked(E, !_.isItemChecked(E));
						$(this).toggleClass("on");
						A._lastShiftClick = null;
					} else {
						var H = this.getAttribute("_position"),
						P = Math.min(H, A._lastclick),
						J = Math.max(H, A._lastclick);
						if (A._lastShiftClick) {
							var D = Math.min(A._lastclick, A._lastShiftClick),
							N = Math.max(A._lastclick, A._lastShiftClick);
							while (D <= N) {
								_.setItemChecked(D, false);
								D++;
							}
						} else {}

						while (P <= J) {
							_.setItemChecked(P, true);
							P++;
						}
						A._lastShiftClick = this.getAttribute("_position");
						FileUtils.repaintListView();
					}
				}
			}
			if (FileUtils.getElementData(E).status == 0) {
				FileUtils._shareDropdownDrawerTrigger(this);
			}
			Page.obtain().resolveToolbarMenu();
		});
	},
	getElementDataByView : function (_, B) {
		var C = null;
		if (_ == null) {
			return C;
		}
		var A = _.getBackedData();
		switch (FileUtils._mViewMode) {
		case FileUtils.VIEW_GRID_ICON:
		case FileUtils.VIEW_GRID_THUMB:
			C = A[Math.floor(B / FileUtils._mGridStep)][B % FileUtils._mGridStep];
			break;
		case FileUtils.VIEW_LIST:
		default:
			C = A[B];
			break;
		}
		return C;
	},
	getElementData : function (A) {
		var B = null;
		switch (FileUtils._mViewMode) {
		case FileUtils.VIEW_GRID_ICON:
		case FileUtils.VIEW_GRID_THUMB:
			if (FileUtils._mInfiniteGridView != null) {
				var _ = FileUtils._mInfiniteGridView.getBackedData();
				B = _[Math.floor(A / FileUtils._mGridStep)][A % FileUtils._mGridStep];
			}
			break;
		case FileUtils.VIEW_LIST:
		default:
			if (FileUtils._mInfiniteListView != null) {
				_ = FileUtils._mInfiniteListView.getBackedData();
				B = _[A];
			}
			break;
		}
		return B;
	},
	_listenFolderOpenTriggers : function (_, B) {
		var A = this;
		$(_).click(function (B) {
			var _ = this;
			FileUtils.anchorFoldOpen(B, A, _);
			if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				return true;
			}
			B.preventDefault();
		});
		if (typeof B != "undefined") {
			$(B).click(function (B) {
				var _ = $(".b-no-ln", this);
				if (_) {
					FileUtils.anchorFoldOpen(B, A, _[0]);
				}
				if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
					return true;
				}
				B.preventDefault();
				return false;
			});
		}
	},
	_listenListViewItemDoubleClick : function (_) {
		var A = this;
		if (typeof _ != "undefined") {
			$(_).dblclick(function (B) {
				FileUtils.anchorFoldOpen(B, A, _);
				if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
					return true;
				}
				B.preventDefault();
				return false;
			});
		}
	},
	anchorFoldOpen : function (D, I, F) {
		var H = D || window.event || arguments[0];
		if ($(F).hasClass("opening")) {
			if (disk.DEBUG) {
				console.log("dbclick return");
			}
			return false;
		}
		$(F).addClass("opening");
		setTimeout(function () {
			$(F).removeClass("opening");
		}, 1000);
		if (H.ctrlKey || H.shiftKey) {
			H.preventDefault();
			return false;
		} else {
			if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				return true;
			}
		}
		if (I._mQuerying) {
			return;
		}
		if (I.isListViewLocked()) {
			return false;
		}
		var C = F.getAttribute("_position");
		if (C != null) {
			C = parseInt(C);
			var E = I.getElementData(C);
			if (E) {
				if (E.isdir) {
					I.switchLayoutMode(FileUtils.MODE_NORMAL);
					I._mPendingRequestShareProperty = (E.property == "-1") ? null : E.property;
					I.deactivateListView(true);
					if (F.className.indexOf("path")) {
						var A = E.path;
					} else {
						A = E.path.slice(0, E.path.lastIndexOf("/"));
					}
					FileUtils.historyNavigate(A == disk.util.DirectoryLocation.DIR_ROOT ? {}

						 : {
						path : A
					});
					if (typeof H.stopPropagation != "undefined") {
						H.stopPropagation();
					} else {
						H.cancelBubble = true;
					}
				} else {
					if (F.className.indexOf("path")) {
						if (this._mMboxPlayer != null && E.category == 2 && H.button == 0) {
							this._mMboxPlayer.shake();
						}
						if (this._mMboxPlayer != null && E.category == 2 && this._mMboxPlayer._mSwitchAni) {
							I.initAddSong(F);
						}
						if (this._mMboxPlayer != null && E.category == 2 && H.button == 1) {
							I.initAddBackSong(F);
						}
						var B = unify.st.Bootstrap.getPageWindow(),
						_ = -1,
						G = null;
						E.category = parseInt(E.category, 10);
						switch (E.category) {
						case Category.MUS:
							_ = unify.app.HomePageWindow.FEATURE_MUSIC_PLAYER_2;
							G = unify.app.HomePageWindow.FEATURE_MUSIC_PLAYER_PATH;
							break;
						default:
							Category[E.category](E, H, C);
							break;
						}
						if (G != null) {
							B.asyncCall(_, G, function () {
								Category[E.category](E, H, C);
							});
						}
						if (E.category != Category.OTH && E.category != Category.MOV && E.category != Category.APP) {
							if (typeof H.stopPropagation != "undefined") {
								H.stopPropagation();
							} else {
								H.cancelBubble = true;
							}
						}
					} else {
						A = E.path.slice(0, E.path.lastIndexOf("/"));
						FileUtils.historyNavigate(A == disk.util.DirectoryLocation.DIR_ROOT ? {}

							 : {
							path : A
						});
					}
				}
			}
		}
		return false;
	},
	clearCurrentDirCache : function () {
		if (this._mDirMgr.atRoot()) {
			this._mLocalCache.removeAll();
		} else {
			this._mLocalCache.descendentlyRemove(this.resolveCurrentDirKey());
		}
	},
	lockActiveRenderingView : function (A) {
		var _ = (this._mViewMode == FileUtils.VIEW_LIST) ? this._mInfiniteListView : this._mInfiniteGridView;
		if (_ == null) {
			return;
		}
		if (A === true) {
			if (!_.locked()) {
				_.lock(true);
			}
		} else {
			_.lock(false);
		}
	},
	prepareFolderBuilder : function (D, E, M, G) {
		var N = disk.ui.FolderBuilder.getDefault();
		if (N.isVisible()) {
			N.focus();
			N.select();
			return;
		}
		var H = this,
		L = null,
		O = null,
		J = null,
		P = null;
		if (D == FileUtils.UPDATE) {
			N.setIgnoreTrailingExt(E.isdir == 1);
			L = M;
			switch (FileUtils._mViewMode) {
			case FileUtils.VIEW_GRID_ICON:
			case FileUtils.VIEW_GRID_THUMB:
				if (!G && !this._mInfiniteGridView.isFirstCheckedChildVisible()) {
					this._mInfiniteGridView.scrollToPositionEx(this._mInfiniteGridView.getFirstCheckedIndex());
					L = this._mInfiniteGridView.getFirstCheckedChild();
				}
				break;
			case FileUtils.VIEW_LIST:
			default:
				if (!G && !this._mInfiniteListView.isFirstCheckedChildVisible()) {
					var R = this._mInfiniteListView.getFirstCheckedIndex(),
					C = this._mInfiniteListView.getCount();
					R = Math.max(1, R);
					if (C > 1) {
						R = Math.min(C - 1, R);
					}
					this._mInfiniteListView.scrollToPosition(R);
					L = this._mInfiniteListView.getFirstCheckedChild();
				}
				break;
			}
			switch (FileUtils._mViewMode) {
			case FileUtils.VIEW_GRID_ICON:
			case FileUtils.VIEW_GRID_THUMB:
				this.setGridViewFileNameVisiable(false, L);
				J = L;
				P = $("a:last", J);
				O = P.text();
				break;
			case FileUtils.VIEW_LIST:
			default:
				J = L.getElementsByTagName("div")[0];
				P = $(".file-handler", J);
				O = P.attr("title");
				P.css("display", "none");
				$(".inline-commands", J).css("display", "none");
				break;
			}
		} else {
			switch (FileUtils._mViewMode) {
			case FileUtils.VIEW_GRID_ICON:
			case FileUtils.VIEW_GRID_THUMB:
				if (H._mInfiniteGridView != null) {
					this._mInfiniteGridView.onSystemNotify(disk.ui.RecycleListView.NOTIFY_LIST_EMPTY, false);
					this._mInfiniteGridView.scrollToPosition(1);
					var K = this._mLocalCache.obtain(this.resolveCurrentDirKey()),
					A = K == null ? [] : K,
					I = Math.round(new Date().getTime() / 1000);
					A.unshift({
						isdir : 1,
						ctime : I,
						modify_time : I,
						path : "",
						size : 0,
						dir_empty : 1,
						empty : 1
					});
					var _ = this.setGroupData(A, this._mGridStep);
					this._onLoadGridView(_);
					var F = this._mInfiniteGridView.getRenderingChildAt(0).childNodes[0],
					Q = F.getElementsByTagName("div")[1];
					this.setGridViewFileNameVisiable(false, F);
					J = F;
				}
				break;
			case FileUtils.VIEW_LIST:
			default:
				if (H._mInfiniteListView != null) {
					this._mInfiniteListView.onSystemNotify(disk.ui.RecycleListView.NOTIFY_LIST_EMPTY, false);
					this._mInfiniteListView.scrollToPosition(1);
					L = document.createElement("dd");
					L.className = "clearfix file-item";
					L.innerHTML = '<div class="file-col col"><span style="display:none" class="inline-commands b-btn clearfix"></span><span class="inline-file-col d"><a href="javascript:;" class="b-in-blk sprite-ic input-cbx"><dfn><s class="sprite"></s></dfn></a><span class="b-in-blk sprite-list-ic b-ic-book"></span></span></div><div class="size-col col"><span>-</span></div><div class="time-col col"><span>-</span></div>';
					var B = this._mInfiniteListView.getRenderingChildAt(0);
					if (B == null) {
						FileUtils._mFileListDOM.appendChild(L);
					} else {
						FileUtils._mFileListDOM.insertBefore(L, B);
					}
					J = L.getElementsByTagName("div")[0];
				}
				break;
			}
		}
		$(L).addClass("auto");
		var S = J.getElementsByTagName("span")[2];
		N.onAttach = function (A, _) {
			if (A) {
				$(".input-cbx", _).addClass("list-cbx-fix");
				$(S).addClass("list-icon-fix");
			} else {
				$(".input-cbx", _).removeClass("list-cbx-fix");
				$(S).removeClass("list-icon-fix");
			}
		};
		N.attach($("a:last", J).parent().get(0), function (I, F) {
			if (H._mQuerying) {
				return;
			}
			var N = this;
			if (I == disk.ui.FolderBuilder.POSITIVE) {
				if (D == FileUtils.UPDATE) {
					if (F == O) {
						N.setVisible(false);
						N.detach();
						switch (FileUtils._mViewMode) {
						case FileUtils.VIEW_GRID_ICON:
						case FileUtils.VIEW_GRID_THUMB:
							H.setGridViewFileNameVisiable(true, L);
							H.lockActiveRenderingView(false);
							break;
						case FileUtils.VIEW_LIST:
						default:
							P.css("display", "");
							P.text(O);
							$(L).removeClass("auto");
							$(".inline-commands", J).css("display", "");
							H.lockActiveRenderingView(false);
							break;
						}
						return;
					}
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_LOADING,
						msg : "\u6b63\u5728\u91cd\u547d\u540d\u6587\u4ef6" + (L.getAttribute("dir") == 1 ? "\u5939" : "") + "&hellip;",
						sticky : true
					});
					var G = E.path,
					A = (FileUtils._mViewMode == FileUtils.VIEW_LIST) ? FileUtils._mInfiniteListView : FileUtils._mInfiniteGridView;
					H._mQuerying = true;
					H.sendUpdateFileMessage(E.isdir == 1, G, F, function (D) {
						H._mQuerying = false;
						try {
							var C = $.parseJSON(D);
							if (C.errno == 0) {
								switch (FileUtils._mViewMode) {
								case FileUtils.VIEW_GRID_ICON:
								case FileUtils.VIEW_GRID_THUMB:
									H.setGridViewFileNameVisiable(true, L);
									break;
								case FileUtils.VIEW_LIST:
								default:
									P.css("display", "");
									break;
								}
								P.text(F);
								H.lockActiveRenderingView(false);
								var B = FileUtils.resolveNewName(L.getAttribute("dir") == 1, G, F, true);
								$(".inline-commands", J).css("display", "");
								$(L).removeClass("auto");
								N.setVisible(false);
								N.detach();
								var _ = disk.Context.getService(disk.Context.SERVICE_TOAST);
								_.setVisible(false);
								if (FileUtils.getModule() == "category" || FileUtils.inSearchMode()) {
									_.pending(function () {
										Utilities.useToast({
											toastMode : disk.ui.Toast.MODE_SUCCESS,
											msg : "重命名成功",
											sticky : false
										});
									});
									if (FileUtils.getModule() == "category") {
										FileUtils.getLocalCache().removeAll();
										FileUtils.getLocalCache().removeCategorys();
										FileUtils.clearAllDocData();
										FileUtils.triggerType(H._mType);
									} else {
										H.pendingHighlights([FileUtils.parseDirPath(B)]);
										FileUtils.getLocalCache().removeAll();
										FileUtils.getLocalCache().removeCategorys();
										FileUtils.clearAllDocData();
										FileUtils.issueProxyLoading(true, false);
										var R = FileUtils.resolveCurrentDirKey();
										FileUtils.loadDir(FileUtils.resolveCurrentDirKey());
									}
									Utilities.useToast({
										toastMode : disk.ui.Toast.MODE_LOADING,
										msg : "\u6b63\u5728\u52a0\u8f7d\u6570\u636e&hellip;",
										sticky : true
									});
								} else {
									var M = disk.util.FileCache.KEY_MODE_NORMAL,
									Q = FileUtils.resolveCurrentDirKey(),
									K = FileUtils.getLocalCache().renameByMode(M, Q, E, B, F);
									if (K) {
										FileUtils.clearAllDocData();
										var I = FileUtils.getListView();
										I.setItemsChecked(false);
										D = FileUtils.getLocalCache().obtainByMode(M, Q);
										if (H._mViewMode != FileUtils.VIEW_LIST) {
											D = H.setGroupData(D, H._mGridStep);
										}
										I.updateBackedData(D);
										Page.obtain().resolveToolbarMenu();
									}
									Utilities.useToast({
										toastMode : disk.ui.Toast.MODE_SUCCESS,
										msg : "\u91cd\u547d\u540d\u6210\u529f",
										sticky : false
									});
								}
							} else {
								if (C.errno == -8 || C.errno == -9 || C.info[0].errno == -8 || C.info[0].errno == -9) {
									var S = "名称已在使用，请使用其他名称";
									if (C.errno == 12 && C.info[0] && C.info[0].errno == -9) {
										S = "重命名失败，文件已被删除或移走";
									}
									Utilities.useToast({
										toastMode : disk.ui.Toast.MODE_CAUTION,
										msg : S,
										sticky : false
									});
								} else {
									N.setVisible(false);
									N.detach();
									$(L).removeClass("auto");
									Utilities.useToast({
										toastMode : disk.ui.Toast.MODE_CAUTION,
										msg : "文件(夹)重命名失败, 网络错误",
										sticky : false
									});
									P.css("display", "");
									P.text(O);
									$(".inline-commands", J).css("display", "");
									H.lockActiveRenderingView(false);
								}
							}
						} catch (T) {
							N.setVisible(false);
							N.detach();
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : "\u6587\u4ef6(\u5939)\u91cd\u547d\u540d\u5931\u8d25, \u7f51\u7edc\u9519\u8bef",
								sticky : false
							});
							P.css("display", "");
							P.text(O);
							$(".inline-commands", J).css("display", "");
							A.lock(false);
						}
					});
				} else {
					var M = H._mDirMgr.getDir() + "/" + F;
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_LOADING,
						msg : "\u6b63\u5728\u521b\u5efa\u6587\u4ef6\u5939&hellip;",
						sticky : true
					});
					H._mQuerying = true;
					H.sendCreateNewFileMessage(M, "", 1, null, function (E) {
						H._mQuerying = false;
						try {
							var D = $.parseJSON(E),
							_ = D.mtime,
							I = D.path,
							F = I;
							if (D.errno == 0) {
								if (FileUtils._mViewMode == FileUtils.VIEW_GRID_ICON || FileUtils._mViewMode == FileUtils.VIEW_GRID_THUMB) {
									var B = H._mLocalCache.obtain(H.resolveCurrentDirKey()),
									A = B == null ? [] : B;
									A.shift();
								}
								var G = FileUtils.resolveParentDirSharedProperty(),
								_ = Math.round(new Date().getTime() / 1000);
								H._mLocalCache.insert(H.resolveCurrentDirKey(), {
									isdir : 1,
									local_ctime : D.ctime,
									server_ctime : D.ctime,
									server_mtime : D.mtime,
									local_mtime : D.mtime,
									path : F,
									status : D.status,
									fs_id : D.fs_id,
									size : 0,
									dir_empty : 1,
									empty : 1
								});
								H._mFileFilter.addRule(FileUtils.parseDirFromPath(I));
								H.resolveParentDirEmptyState(H.resolveCurrentDirKey());
								H.pendingHighlights([FileUtils.parseDirPath(I)]);
								N.setVisible(false);
								N.detach();
								switch (FileUtils._mViewMode) {
								case FileUtils.VIEW_GRID_ICON:
								case FileUtils.VIEW_GRID_THUMB:
									if (H._mInfiniteGridView != null) {
										var C = H._mInfiniteGridView.getRenderingChildAt(0).childNodes[0];
										H._mInfiniteGridView.setItemsChecked(false);
										H.setGridViewFileNameVisiable(true, C);
										H.lockActiveRenderingView(false);
										FileUtils.load(FileUtils.resolveCurrentDirKey());
									}
									break;
								case FileUtils.VIEW_LIST:
								default:
									if (H._mInfiniteListView != null) {
										L.parentNode.removeChild(L);
										H.lockActiveRenderingView(false);
										FileUtils.load(FileUtils.resolveCurrentDirKey());
									}
									break;
								}
								Utilities.useToast({
									toastMode : disk.ui.Toast.MODE_SUCCESS,
									msg : "\u521b\u5efa\u6587\u4ef6\u5939\u6210\u529f",
									sticky : false
								});
							} else {
								if (D.errno == -8) {
									Utilities.useToast({
										toastMode : disk.ui.Toast.MODE_CAUTION,
										msg : "\u540d\u79f0\u5df2\u5728\u4f7f\u7528\uff0c\u8bf7\u4f7f\u7528\u5176\u4ed6\u540d\u79f0",
										sticky : false
									});
								} else {
									N.setVisible(false);
									N.detach();
									if (FileUtils._mViewMode == FileUtils.VIEW_LIST) {
										L.parentNode.removeChild(L);
									}
									H.lockActiveRenderingView(false);
									Utilities.useToast({
										toastMode : disk.ui.Toast.MODE_CAUTION,
										msg : D.errno == 102 ? "\u7b2c\u4e09\u65b9\u5e94\u7528\u76ee\u5f55\u4e0b\u4e0d\u5141\u8bb8\u521b\u5efa\u76ee\u5f55" : "\u6587\u4ef6\u5939\u521b\u5efa\u5931\u8d25, \u7f51\u7edc\u9519\u8bef",
										sticky : false
									});
								}
							}
						} catch (J) {
							N.setVisible(false);
							N.detach();
							if (FileUtils._mViewMode == FileUtils.VIEW_LIST) {
								L.parentNode.removeChild(L);
							}
							H.lockActiveRenderingView(false);
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : "\u6587\u4ef6\u5939\u521b\u5efa\u5931\u8d25, \u7f51\u7edc\u9519\u8bef",
								sticky : false
							});
						}
					});
				}
			} else {
				if (I == disk.ui.FolderBuilder.ILLEGAL) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u6587\u4ef6(\u5939)\u540d\u683c\u5f0f\u4e0d\u6b63\u786e",
						sticky : false
					});
				} else {
					switch (FileUtils._mViewMode) {
					case FileUtils.VIEW_GRID_ICON:
					case FileUtils.VIEW_GRID_THUMB:
						H.lockActiveRenderingView(false);
						N.setVisible(false);
						N.detach();
						if (D != FileUtils.UPDATE) {
							var C = H._mLocalCache.obtain(H.resolveCurrentDirKey()),
							B = C == null ? [] : C;
							B.shift();
							var _ = H.setGroupData(B, H._mGridStep);
							H._onLoadGridView(_);
							if (_.length != 0) {
								var K = H._mInfiniteGridView.getRenderingChildAt(0).childNodes[0];
								H._mInfiniteGridView.setItemsChecked(false);
								H.setGridViewFileNameVisiable(true, K);
							}
						} else {
							H.setGridViewFileNameVisiable(true, L);
						}
						if (H._mInfiniteGridView.getCount() == 0) {
							H._mInfiniteGridView.onSystemNotify(disk.ui.RecycleListView.NOTIFY_LIST_EMPTY, true);
						}
						break;
					case FileUtils.VIEW_LIST:
					default:
						H.lockActiveRenderingView(false);
						N.setVisible(false);
						N.detach();
						if (D != FileUtils.UPDATE) {
							L.parentNode.removeChild(L);
						} else {
							$(".inline-commands", J).css("display", "");
							P.css("display", "");
							$(L).removeClass("auto");
						}
						if (H._mInfiniteListView.getCount() == 0) {
							H._mInfiniteListView.onSystemNotify(disk.ui.RecycleListView.NOTIFY_LIST_EMPTY, true);
						}
						break;
					}
				}
			}
		}).setVisible(true);
		N.setUseShortStyle(false);
		if (FileUtils._mViewMode == FileUtils.VIEW_GRID_ICON) {
			(N._mTextField).attr("size", 1);
		} else {
			if (this._mViewMode == FileUtils.VIEW_GRID_THUMB) {
				(N._mTextField).attr("size", 1);
				N._mTextField.css("width", "100px");
			} else {
				N._mTextField.removeAttr("style");
				N.setUseShortStyle(false);
			}
		}
		if (O != null) {
			N.setText(O);
		}
		N.focus();
		N.select();
		H.lockActiveRenderingView(true);
	},
	setGridViewFileNameVisiable : function (B, C) {
		if (FileUtils._mViewMode == FileUtils.VIEW_GRID_ICON || FileUtils._mViewMode == FileUtils.VIEW_GRID_THUMB) {
			var A = C.getElementsByTagName("a")[0],
			_ = C.getElementsByTagName("a")[1];
			if (B) {
				$(A).css("display", "");
				$(_).css("display", "");
			} else {
				$(A).css("display", "none");
				$(_).css("display", "none");
			}
		}
	},
	resolveParentDirSharedProperty : function (_) {
		return -1;
	},
	resolveParentDirEmptyState : function (E) {
		if (E == disk.util.DirectoryLocation.DIR_ROOT) {
			return;
		}
		var D = FileUtils.resolveDirKey(E.substring(0, E.lastIndexOf("/"))),
		B = this._mLocalCache.obtain(D);
		if (!B) {
			return;
		}
		for (var C = 0, A = B.length; C < A; C++) {
			var _ = B[C];
			if (E == FileUtils.parseDirPath(_.path)) {
				_.empty = 0;
				_.dir_empty = 0;
				if (disk.DEBUG) {
					console.log("found a parent dir and set it to not empty state");
				}
				break;
			}
		}
	},
	resolveNewName : function (B, E, C, A) {
		var _ = E.substring(0, E.lastIndexOf("/")) + "/",
		D = B ? C : C;
		if (A) {
			D = _ + D;
		}
		return D;
	},
	sendCreateNewFileMessage : function (B, A, D, C, _) {
		$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.CREATE, FileUtils.bdstoken), {
			path : B,
			isdir : D,
			size : A,
			block_list : C ? '["' + C + '"]' : "[]",
			method : "post"
		}, _);
	},
	sendUpdateFileMessage : function (A, C, B, _) {
		C = this.parseDirPath(C);
		$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.FILE_MANAGER + "&opera=rename", FileUtils.bdstoken), {
			filelist : $.stringify([{
						path : C,
						newname : this.resolveNewName(A, C, B)
					}
				])
		}, _);
	},
	sendSearchMessage : function (B, A, _) {
		$.get(disk.api.RestAPI.SEARCH + "&dir=&key=" + encodeURIComponent(A) + "&recursion&timeStamp=" + Math.random(), _);
	},
	sendMoveCopyFileMessage : function (J, E, I, D) {
		var F = [],
		H = null,
		C = [],
		A,
		G;
		for (var B = 0, _ = E.length; B < _; B++) {
			H = E[B].path;
			A = this.parseDirPath(H);
			G = this.parseDirFromPath(H);
			F.push({
				path : A,
				dest : I,
				newname : G
			});
			C.push(I == "/" ? I + G : I + "/" + G);
		}
		this.pendingHighlights(C);
		$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.FILE_MANAGER + "&opera=" + (J ? "move" : "copy"), FileUtils.bdstoken), {
			filelist : $.stringify(F)
		}, D);
	},
	resolveFileIcon : function (B, A, _) {
		var E = "",
		F,
		C,
		D = A + 1;
		(D & 1) && (E += "share-");
		(D & 2) && (E += "public-");
		filetype = B ? "d" : "f";
		E += filetype;
		!B && _ && (E += " f" + (Category.iconMap[_.toLowerCase()] || Category.OTH));
		return E;
	},
	resolveFileSmallIcon : function (D, J, C, _, A, F) {
		var H = "",
		I = 0,
		E = 0,
		G = 0;
		if (_) {
			if (J && J.constructor == Array && ((G = J.length) > 1 || G == 0)) {
				_.css("backgroundPosition", G == 0 ? "-38px -48px" : "-38px -24px");
				return;
			}
			if (D) {
				if (A == FileUtils.PART_KID_FOLDER) {
					switch (FileUtils.PARTICULAR_DIR_MAP[F]) {
					case FileUtils.PARTICULAR_DIR_MAP["/apps"]:
						_.css("backgroundPosition", "-38px 0px");
						break;
					case FileUtils.PARTICULAR_DIR_MAP["/\u6765\u81eaPC\u7684\u5907\u4efd\u6587\u4ef6"]:
						_.css("backgroundPosition", "-116px 0px");
						break;
					case FileUtils.PARTICULAR_DIR_MAP["/\u767e\u5ea6\u4e91\u6536\u85cf"]:
						_.css("backgroundPosition", "-197px 0px");
						break;
					case FileUtils.PARTICULAR_DIR_MAP["/\u6211\u7684\u5206\u4eab"]:
						_.css("backgroundPosition", "-238px 0px");
						break;
					case FileUtils.PARTICULAR_DIR_MAP["/\u6211\u7684\u4f5c\u54c1"]:
						_.css("backgroundPosition", "-278px 0px");
						break;
					default:
						_.css("backgroundPosition", "-38px 0px");
						break;
					}
					return;
				}
				E = 0;
			} else {
				E = -80;
			}
			if (!D && C) {
				var B = Category.iconMap[C.toLowerCase()] || Category.OTH,
				I = Category.iconMap_X[B];
				switch (B) {
				case 70:
					E = -102;
					break;
				}
			}
			_.css("backgroundPosition", I + "px " + E + "px");
		}
	},
	resolveFileBigIcon : function (A, _) {
		var B = "tl-";
		B += A ? "folder" : "file";
		!A && _ && (B += " f" + (Category.iconMap[_.toLowerCase()] || Category.OTH));
		return B;
	},
	resolveFileGridIcon : function (D, C, B, _, A, F, G, E) {
		if (D) {
			if (A == FileUtils.PART_KID_FOLDER) {
				switch (FileUtils.PARTICULAR_DIR_MAP[G]) {
				case FileUtils.PARTICULAR_DIR_MAP["/apps"]:
					$(_).parent().css("backgroundPosition", "-159px 0px");
					break;
				case FileUtils.PARTICULAR_DIR_MAP["/\u6765\u81eaPC\u7684\u5907\u4efd\u6587\u4ef6"]:
					$(_).parent().css("backgroundPosition", "-317px 0");
					break;
				case FileUtils.PARTICULAR_DIR_MAP["/\u767e\u5ea6\u4e91\u6536\u85cf"]:
					$(_).parent().css("backgroundPosition", "-798px 0");
					break;
				case FileUtils.PARTICULAR_DIR_MAP["/\u6211\u7684\u5206\u4eab"]:
					$(_).parent().css("backgroundPosition", "-959px 0");
					break;
				case FileUtils.PARTICULAR_DIR_MAP["/\u6211\u7684\u4f5c\u54c1"]:
					$(_).parent().css("backgroundPosition", "-1122px 0");
					break;
				default:
					$(_).parent().css("backgroundPosition", "0 0");
					break;
				}
			} else {
				$(_).parent().css("backgroundPosition", "0 0");
			}
			return "tl-folder";
		}
		var I = "tl-",
		J = y = 0,
		H = E.thumbs && E.thumbs.url1 || (F + "&method=view&type=jpg&height=82&quality=100");
		bcolor = "#d1d1d1";
		positionStep = 0;
		p = C + 1;
		switch (FileUtils._mViewMode) {
		case FileUtils.VIEW_GRID_THUMB:
			positionStep = 153;
			break;
		case FileUtils.VIEW_GRID_ICON:
		default:
			positionStep = 153;
			break;
		}
		I += D ? "folder" : "file";
		!D && B && (I += " f" + (Category.iconMap[B.toLowerCase()] || Category.OTH));
		if (_) {
			if (D) {
				J = -1;
				y = -5;
				(p == 1) && (y = -2 * positionStep - 5);
				(p == 2) && (y = -positionStep - 5);
				(p == 3) && (y = -positionStep - 5);
			} else {
				J = 0;
				y = 0;
				(p == 1) && (y = -104);
				(p == 2) && (y = -208);
				(p == 3) && (y = -208);
			}
			if (FileUtils._mViewMode == FileUtils.VIEW_GRID_THUMB) {
				y += 5;
			}
			if (!D && B) {
				switch (Category.iconMap[B.toLowerCase()] || Category.OTH) {
				case Category.Excel:
					J = -2 * positionStep;
					bcolor = "#bce5bb";
					break;
				case Category.Word:
					J = -3 * positionStep;
					bcolor = "#c9ecf8";
					break;
				case Category.Ppt:
					J = -4 * positionStep;
					bcolor = "#f1d8a3";
					break;
				case Category.Visio:
					J = -5 * positionStep;
					bcolor = "#d7e0ee";
					break;
				case Category.Pdf:
					J = -6 * positionStep;
					bcolor = "#efdddd";
					break;
				case Category.Doc_oth:
				case Category.Txt:
					J = -7 * positionStep;
					bcolor = "#e5e5e5";
					break;
				case Category.Exe:
					J = -9 * positionStep;
					bcolor = "#cee5ea";
					break;
				case Category.Apk:
					J = -8 * positionStep;
					bcolor = "#bce5bb";
					break;
				case Category.Ipa:
					J = -10 * positionStep;
					bcolor = "#d1d1d1";
					break;
				case Category.MOV:
					J = -11 * positionStep;
					bcolor = "#b9dcff";
					break;
				case Category.MUS:
					J = -12 * positionStep;
					bcolor = "#f5c077";
					break;
				case Category.Torrent:
					J = -13 * positionStep;
					bcolor = "#f5c077";
					break;
				case Category.Zip:
					J = -1 * positionStep;
					bcolor = "#dad0fa";
					break;
				case Category.Rar:
					J = -14 * positionStep;
					bcolor = "#dad0fa";
					break;
				case Category.Psd:
					J = -15 * positionStep;
					bcolor = "#007acb";
					break;
				case Category.Key:
					J = -16 * positionStep;
					bcolor = "#007acb";
					break;
				case Category.Dmg:
					J = -17 * positionStep;
					bcolor = "#007acb";
					break;
				case Category.Ai:
					J = -18 * positionStep;
					bcolor = "#007acb";
					break;
				case Category.Cad:
					y = -96;
					break;
				case Category.Mmap:
					J = -1 * positionStep;
					y = -95;
					break;
				case Category.Xmind:
					J = -2 * positionStep;
					y = -96;
					break;
				case Category.Mm:
					J = -3 * positionStep;
					y = -96;
					break;
				}
			}
			$(_).css("backgroundPosition", J + "px " + y + "px");
			$(_).next(".file-name").css("borderColor", "#c6c6c6");
		}
		return I;
	},
	scheduleHidingHighlight : function () {
		if (this._mHighlightTimer == null) {
			this._mHighlightTimer = new disk.util.TimerService(3000, function () {
					$(FileUtils._mFileListDOM).removeClass("highlight-on");
					$(FileUtils._mFileGridDOM).removeClass("highlight-on");
				});
		}
		if (this._mHighlightTimer.isAlive()) {
			this._mHighlightTimer.interrupt();
		}
		this._mHighlightTimer.start();
	},
	isListViewLocked : function () {
		if (this._mViewMode == FileUtils.VIEW_LIST) {
			return (this._mInfiniteListView != null) ? this._mInfiniteListView.locked() : false;
		} else {
			return this._mInfiniteGridView != null ? this._mInfiniteGridView.locked() : false;
		}
	},
	_translatePendingFilesIndexes : function (A) {
		var F = [],
		G = this._mPendingHighlights,
		C = -1;
		if (G.length == 0 || A.length == 0) {
			return;
		}
		var E;
		for (var B = 0, _ = A.length; B < _; B++) {
			E = this.parseDirPath(A[B].path);
			for (var D = 0, H = G.length; D < H; D++) {
				if (E == G[D]) {
					F[B] = true;
					if (C == -1) {
						C = B;
					}
				}
			}
		}
		this._mPendingHighlightIndexes = (F.length > 0) ? F : null;
		this._mPendingHighlightFirst = C;
		if (disk.DEBUG) {
			console.log("[LOG]translate selection ", this._mPendingHighlightIndexes);
		}
	},
	setInnerText : (typeof window.attachEvent != "undefined") ? function (A, _) {
		A.innerText = _;
	}
	 : function (A, _) {
		A.textContent = _;
	},
	setGroupData : function (A, F) {
		var D = A == null ? 0 : A.length;
		if (D == 0) {
			return [];
		}
		var C = new Array(),
		_ = 0;
		while (D > F) {
			C[_] = [];
			for (var B = 0; B < F; B++) {
				C[_].push(A[_ * F + B]);
			}
			_++;
			D -= F;
		}
		if (D > 0) {
			C[_] = [];
		}
		var E = 0;
		while (D > 0) {
			C[_].push(A[_ * F + E]);
			D--;
			E++;
		}
		return C;
	},
	getFileListDOM : function () {
		return this._mFileListDOM;
	},
	_onLoadGridView : function (_, A) {
		this.setDisplaySortColsHeader(false);
		if (this._mInfiniteGridView == null) {
			var B = this;
			this._mInfiniteGridView = new disk.ui.RecycleListView({
					listContainer : document.getElementById("infiniteGridView"),
					tracker : document.getElementById("scrollbarTrackerGrid"),
					thumb : document.getElementById("scrollbarThumbGrid"),
					upArrow : document.getElementById("scrollbarArrowUpGrid"),
					downArrow : document.getElementById("scrollbarArrowDownGrid"),
					scrollbar : document.getElementById("scrollbarGrid")
				}, {
					flags : disk.ui.RecycleListView.USING_MOUSE_WHEEL_SENSOR | disk.ui.RecycleListView.USING_SCROLLBAR | disk.ui.RecycleListView.USING_KEYBOARD_DISPATCHER
				});
			if (this._mInfiniteListView && this._mInfiniteListView.isAllItemChecked()) {
				this._mInfiniteGridView.setItemsChecked(true);
			}
			this._mInfiniteGridView.gridStep = this._mGridStep;
			this._mInfiniteGridView.setDebugAlias("GridView");
			this._mInfiniteGridView.getElementsData = function () {
				var D = [];
				for (var A = 0, _ = this._mElementsData.length; A < _; A++) {
					for (var B = 0, C = this._mElementsData[A].length; B < C; B++) {
						D.push(this._mElementsData[A][B]);
					}
				}
				return D;
			};
			this._mInfiniteGridView.setItemChecked = function (B, C) {
				this._mCheckedChildren[B] = C;
				var D = (this._mPrivateFlags & disk.ui.RecycleListView.CHECKED_ALL) == disk.ui.RecycleListView.CHECKED_ALL;
				if (!C && D) {
					this._mPrivateFlags &= ~disk.ui.RecycleListView.CHECKED_ALL;
					for (var A = 0, _ = this.getElementsData().length; A < _; A++) {
						if (A != B) {
							this._mCheckedChildren[A] = true;
						}
					}
				}
			};
			this._mInfiniteGridView.getCheckedItems = function () {
				var E = [];
				if (!this._mElementsData) {
					return E;
				}
				if (this.isAllItemChecked()) {
					for (var A = 0, _ = this._mElementsData.length; A < _; A++) {
						for (var D = 0, F = this._mElementsData[A].length; D < F; D++) {
							E.push(this._mElementsData[A][D]);
						}
					}
					return E;
				}
				var B = this._mElementsData.length;
				for (A = 0, _ = this._mCheckedChildren.length; A < _; A++) {
					if (this._mCheckedChildren[A] === true) {
						var C = Math.floor(A / FileUtils._mGridStep);
						if (C >= B) {
							break;
						}
						E.push(this._mElementsData[C][A % FileUtils._mGridStep]);
					}
				}
				return E;
			};
			this._mInfiniteGridView.getFirstCheckedChild = function () {
				for (var A = 0, _ = this._mCheckedChildren.length; A < _; A++) {
					if (this._mCheckedChildren[A] === true) {
						if (Math.floor(A / FileUtils._mGridStep) >= this._mFirstPosition && Math.abs(Math.floor(A / FileUtils._mGridStep) - this._mFirstPosition) < this._mChildren.length) {
							return this._mChildren[Math.floor(A / FileUtils._mGridStep) - this._mFirstPosition].childNodes[A % FileUtils._mGridStep];
						} else {
							return null;
						}
					}
				}
				return null;
			};
			this._mInfiniteGridView.getView = function (U, G, C) {
				var _ = G;
				if (_ == null) {
					_ = this.buildView(U, G, C);
				}
				var S = FileUtils._mGridStep,
				E = _.getElementsByTagName("div"),
				H = this._mElementsData[C];
				for (var K = 0; K < H.length; ++K) {
					E[4 * K].style.display = "block";
					E[4 * K].className = "clearfix icon-item";
					switch (FileUtils._mViewMode) {
					case FileUtils.VIEW_GRID_THUMB:
						$(E[4 * K]).addClass("icon-item-big");
						break;
					case FileUtils.VIEW_GRID_ICON:
					default:
						$(E[4 * K]).removeClass("icon-item-big");
						break;
					}
					E[4 * K].setAttribute("_position", C * S + K);
					iconDiv = E[2 + 4 * K];
					$(iconDiv).removeAttr("style");
					var D = this._mElementsData[C][K],
					R = D.path,
					P = FileUtils.getNormalizedPath(FileUtils.parseDirPath(R), true);
					D.isdir = D.isdir - 0;
					var A = null;
					if (FileUtils.isInParticularDir(FileUtils.parseDirPath(R))) {
						A = FileUtils.PART_KID_FOLDER;
						if (D.category == "3") {
							A = FileUtils.PART_KID_IMAGE;
						}
					}
					var F = FileUtils.resolveFileGridIcon(D.isdir, D.property, R.slice(R.lastIndexOf(".")), iconDiv, A, D.docpreview, R, D),
					J = null;
					if (F.indexOf("tl-file f3") != -1) {
						if (D.thumbs != null) {
							switch (FileUtils._mViewMode) {
							case FileUtils.VIEW_GRID_THUMB:
								J = D.thumbs.url2;
								break;
							case FileUtils.VIEW_GRID_ICON:
							default:
								J = D.thumbs.url1.replace("c140_u90", "c94_u94");
								break;
							}
						}
						if (J == null || J == "") {
							$(iconDiv).css({
								background : "url(/res/static/images/default_new.png) 50% 50% no-repeat",
								border : "#bebebe 1px solid",
								width : "94px",
								height : "94px"
							});
							$(iconDiv).next(".file-name").css({
								borderColor : "#bebebe"
							});
						} else {
							$(iconDiv).css({
								background : "url(" + J + ") 50% 50% no-repeat",
								width : "94px",
								height : "94px"
							});
							$(iconDiv).next(".file-name").css({
								borderColor : "#bebebe"
							});
						}
					}
					var Q = $(iconDiv);
					Q.find(".grid-view-video-thumb").remove();
					Q.find(".grid-view-video-arrow").remove();
					var T = Q.next(".file-name").removeClass("video").removeClass("grid-view-video-name-icon");
					if (D.category == Category.MOV && (Page.inViewMode(Page.VIEW_HOME) || Page.inViewMode(Page.VIEW_PROPERTY_OTHER))) {
						if (Q.find(".grid-view-video-thumb").length == 0) {
							Q.css({
								position : "relative"
							});
							var I = disk.ui.Utilities.getThumbnail(D, {
									width : 105,
									height : 84
								});
							$('<span class="grid-view-video-thumb"></span>').css({
								background : '50% 50% no-repeat url("' + I + '")'
							}).appendTo(Q);
							if (Page.inViewMode(Page.VIEW_HOME) || Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
								$('<span class="grid-view-video-arrow sprite"></span>').appendTo(Q);
							}
						}
						T.addClass("grid-view-video-name-icon");
					}
					if (D.isdir) {
						$(iconDiv).parent().addClass("tl-folder");
						iconDiv.className = "thumb-large";
					} else {
						$(iconDiv).parent().removeClass("tl-folder");
						iconDiv.className = "thumb-grid " + F;
					}
					iconDiv.style.cursor = "pointer";
					if (D.isdir) {}
					else {}

					fileDiv = E[3 + 4 * K];
					var N = fileDiv.getElementsByTagName("a"),
					M = N[0],
					L = N[1];
					if (D.isdir || D.category == "4" || D.category == "2" || D.category == "3" || D.category == Category.MOV || FileUtils.acceptOpenFileTypes(R.slice(R.lastIndexOf(".")))) {
						L.style.cursor = "pointer";
					} else {
						L.style.cursor = "default";
						L.style.textDecoration = "none";
					}
					M.setAttribute("_position", C * S + K);
					L.setAttribute("_position", C * S + K);
					M.parentNode.parentNode.title = P;
					if (this.isItemChecked(C * S + K)) {
						M.className = "b-in-blk input-cbx";
						$(E[4 * K]).addClass("on");
					} else {
						M.className = "b-in-blk input-cbx";
						$(E[4 * K]).removeClass("on");
					}
					B.setInnerText(L, P);
					if (!E[4 * K].getAttribute("_installed")) {
						B._listenCheckboxTriggers(M);
						B._listenClickTriggers(E[4 * K]);
						FileUtils._listenMouseMovement(E[4 * K]);
						B._listenMouseRightMenuTriggers(E[4 * K]);
						if (!Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
							B._listenFolderOpenTriggers(L, E[4 * K]);
							B._listenDragFileTriggers(E[4 * K]);
						}
						E[4 * K].setAttribute("_installed", "1");
					}
					if (H.length != S) {
						for (var O = H.length; O < S; ++O) {
							E[4 * O].style.display = "none";
						}
					}
					if (B._mPendingHighlightIndexes != null && B._mPendingHighlightIndexes[C * S + K] === true) {
						$(E[4 * K]).addClass("highlight");
					} else {
						$(E[4 * K]).removeClass("highlight");
					}
				}
				return _;
			};
			this._mInfiniteGridView.buildView = function (F, G, C) {
				var D = document.createElement("dd");
				D.className = "clearfix";
				var E = FileUtils._mGridStep,
				A = [];
				for (var _ = 0; _ < E; ++_) {
					var B = ['<div><div class="thumb-holder"><div class="thumb-large"><em class="ic-shr-plc"></em><s class="file-3-party"></s></div>'];
					B.push('<div class="file-name"><a hidefocus="true" unselectable="on" href="javascript:;" class="b-in-blk input-cbx"><dfn><s class="sprite"></s></dfn></a><a class="b-no-ln file-handler" href="javascript:;"></a></div></div></div>');
					A.push(B.join(""));
				}
				D.innerHTML = A.join("");
				return D;
			};
			this._mInfiniteGridView.isFirstCheckedChildVisible = function () {
				for (var A = 0, _ = this._mCheckedChildren.length; A < _; A++) {
					if (this.isItemChecked(A)) {
						return (A / FileUtils._mGridStep) >= this._mFirstPosition && (A / FileUtils._mGridStep) < (this._mFirstPosition + this._mChildren.length);
					}
				}
				return false;
			};
			this._mInfiniteGridView.scrollToPositionEx = function (_) {
				var A = Math.floor(_ / B._mGridStep) + 1;
				if (A >= this._mElementsData.length) {
					A = this._mElementsData.length - 1;
				}
				A = Math.max(1, A);
				return this.scrollToPosition(A);
			};
			this._mInfiniteGridView.getCountEx = function () {
				var B = 0;
				if (this._mElementsData == null) {
					return B;
				}
				for (var A = 0, _ = this._mElementsData.length; A < _; A++) {
					B += this._mElementsData[A].length;
				}
				return B;
			};
			this._mInfiniteGridView.onSystemNotify = this._onSystemNotify;
			this._mInfiniteGridView.activateKeyguard(true);
			this._mInfiniteGridView.onComputeScrollbarState = function (D, A, _) {
				disk.ui.RecycleListView.prototype.onComputeScrollbarState.call(this, D, A, _);
				if (A > 0 && this.getCount() > 0 && !B._mQuerying) {
					var C = parseFloat((A + _) / this.getCount(), 10);
					if (C > 0.7) {
						var E = null;
						if (FileUtils.inSearchMode()) {}
						else {
							if (B._mModule == "category") {
								E = B._mCategoryProxyLoader;
							} else {
								E = B._mProxyLoader;
							}
						}
						if (E && !E.isIdle()) {
							E.load();
						}
						if (E) {
							E.endShowMsg(true);
						}
					}
				}
			};
			this._mInfiniteGridView.getScrollbar(true).awake(true);
			if (A === true) {
				this.importCheckedStateBetweenViews();
			}
			this._mInfiniteGridView.setBackedData(_);
		} else {
			if (this._mInfiniteGridView.locked()) {
				this._mInfiniteGridView.lock(false, true);
			}
			this._mInfiniteGridView.awakeScrollbar(true);
			if (A === true) {
				this.importCheckedStateBetweenViews();
				this._mInfiniteGridView.setBooleanFlag(disk.ui.RecycleListView.PRESERVE_CHECKED_STATE, true);
				this._mInfiniteGridView.dispatchDataChanged();
				this._mInfiniteGridView.setBackedData(_);
				this._mInfiniteGridView.setBooleanFlag(disk.ui.RecycleListView.PRESERVE_CHECKED_STATE, false);
			} else {
				this._mInfiniteGridView.changeBackedData(_, true);
			}
		}
	},
	setDisplaySortColsHeader : function (_) {
		if ($("#fileActionHeader").css("display") !== "none" || _ == this._mSortColsDisplayed) {
			return;
		}
		$(".files-header").hide();
		if (_) {
			$("#sortColsHeader").show();
		} else {
			$("#fileThumbHeader").show();
		}
		this._mSortColsDisplayed = _;
		Utilities.resolveLayout();
	},
	importCheckedStateBetweenViews : function (_) {
		if ((this._mViewMode == FileUtils.VIEW_GRID_ICON && this._mLastViewMode == FileUtils.VIEW_GRID_THUMB) || (this._mViewMode == FileUtils.VIEW_GRID_THUMB && this._mLastViewMode == FileUtils.VIEW_GRID_ICON)) {
			if (disk.DEBUG) {
				console.log("Fall into same view mode, no need to import checked state");
			}
			return;
		}
		var B = this._mInfiniteListView,
		A = this._mInfiniteGridView;
		if (B == null || A == null) {
			return;
		}
		if (_ === true) {
			B.importCheckedState(A);
		} else {
			A.importCheckedState(B);
		}
	},
	_onLoadListView : function (_, A) {
		var B = this;
		this.setDisplaySortColsHeader(true);
		if (this._mInfiniteListView == null) {
			this._mInfiniteListView = new disk.ui.RecycleListView({
					listContainer : document.getElementById("infiniteListView"),
					tracker : document.getElementById("scrollbarTracker"),
					thumb : document.getElementById("scrollbarThumb"),
					upArrow : document.getElementById("scrollbarArrowUp"),
					downArrow : document.getElementById("scrollbarArrowDown"),
					scrollbar : document.getElementById("scrollbar")
				}, {
					flags : disk.ui.RecycleListView.USING_MOUSE_WHEEL_SENSOR | disk.ui.RecycleListView.USING_SCROLLBAR | disk.ui.RecycleListView.USING_KEYBOARD_DISPATCHER | disk.ui.RecycleListView.USING_TOUCH_SENSOR
				});
			if (this._mInfiniteGridView && this._mInfiniteGridView.isAllItemChecked()) {
				this._mInfiniteListView.setItemsChecked(true);
			}
			this._mInfiniteListView.getView = (function () {
				var _ = function (M, E, _, J, K, A) {
					_ = E;
					if (_ == null) {
						_ = this.buildView(M, E, A);
					}
					_.setAttribute("_position", A);
					_.className = "clearfix file-item";
					var F = _.getElementsByTagName("span"),
					C = 1,
					N = F[C];
					if (N.className.indexOf("inline-file-col") == -1) {
						C++;
						N = F[C];
					}
					var L = F[C + 1];
					L.style.cursor = "pointer";
					var I = N.parentNode.parentNode.getElementsByTagName("a"),
					H = I[1];
					J == "\u5206\u4eab\u7684\u6587\u4ef6\u5df2\u88ab\u5220\u9664" ? $(H).css("color", "#aaa") : $(H).css("color", "#000");
					$(H).attr("title", J);
					FileUtils.setInnerText(H, J);
					pathHandler = I[2];
					var G = I[0];
					H.setAttribute("_position", A);
					pathHandler.setAttribute("_position", A);
					G.setAttribute("_position", A);
					L.setAttribute("_position", A);
					G.parentNode.parentNode.title = J;
					var D = F[0].getElementsByTagName("a");
					if (!_.getAttribute("_cmd_installed")) {
						FileUtils._listenMouseMovement(_);
						B._listenMouseRightMenuTriggers(_);
						_.setAttribute("_cmd_installed", 1);
					}
					if (this.isAllItemChecked() || this.isItemChecked(A)) {
						G.className = "b-in-blk input-cbx";
						$(_).addClass("on");
					} else {
						G.className = "b-in-blk input-cbx";
						$(_).removeClass("on");
					}
					if (B._mPendingHighlightIndexes != null && B._mPendingHighlightIndexes[A] === true) {
						$(_).addClass("highlight");
					} else {
						$(_).removeClass("highlight");
					}
					return {
						view : _,
						txtHandler : H,
						pathHandler : pathHandler,
						iconSpan : L,
						checkbox : G,
						spans : F,
						txtSpan : N,
						spanStart : C
					};
				};
				if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
					return function (M, I, C) {
						var N = disk.ui.ShareDialog,
						T = this._mElementsData[C],
						L = T.typicalPath || T.path,
						e = Utilities.toDateString(T.server_mtime || T.ctime),
						f = FileUtils.getNormalizedPath(FileUtils.parseDirPath(L), true),
						h = T.vCnt,
						F = T.dCnt,
						U = T.tCnt,
						K = T.passwd,
						O = T.channelInfo,
						G = T.status,
						g = Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) ? FileUtils.sysUK_Master : FileUtils.sysUK,
						X = Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) ? encodeURI(disk.getParam("third")) : FileUtils.third,
						c;
						if (T.shorturl) {
							c = "http://" + location.host + "/s/" + T.shorturl;
						} else {
							c = "http://" + location.host + "/share/link?shareid=" + T.shareId + "&uk=" + g;
						}
						c = X == 0 ? c : c + "&third=" + X;
						T.public = T.public || 0;
						if (T.fsIds.length > 1) {
							f += "\u7b49";
						}
						var Y = "";
						if (T.public == FileUtils.SHARE_PUBLIC_LINK) {
							if (T.channel == FileUtils.SHARE_CHANNEL_PUBLIC_SNS) {
								Y = "\u516c\u5f00\u5206\u4eab";
							} else {
								Y = "\u516c\u5f00\u94fe\u63a5";
							}
						} else {
							if (T.channel == FileUtils.SHARE_CHANNEL_MAIL) {
								Y = "\u90ae\u4ef6";
							} else {
								if (T.channel == FileUtils.SHARE_CHANNEL_SNS) {
									Y = "\u77ed\u4fe1";
								} else {
									Y = "\u79c1\u5bc6\u94fe\u63a5";
								}
							}
						}
						var P = '<a class="sl-a" target="_blank" href=\'' + c + "'>" + c + "</a>",
						d = _.call(this, M, I, Q, f, L, C),
						Q = d.view,
						Z = d.txtHandler,
						i = d.iconSpan,
						a = d.checkbox,
						H = d.spans,
						j = d.txtSpan,
						V = d.spanStart;
						Z.style.cursor = "pointer";
						if (!Z.getAttribute("_installed")) {
							B._listenFolderOpenTriggers(Z);
							B._listenFolderOpenTriggers(pathHandler);
							B._listenFolderOpenTriggers(i);
							B._listenListViewItemDoubleClick(Q);
							B._listenClickTriggers(Q, G);
							B._listenCheckboxTriggers(a);
							B._listenCheckboxDoubleClickTriggers(a);
							Z.setAttribute("_installed", "1");
						}
						var A = "";
						H[3].innerHTML = e;
						H[4].innerHTML = h + "\u6b21";
						H[5].innerHTML = U + "\u6b21";
						H[6].innerHTML = F + "\u6b21";
						if (G == 0) {
							H[9].innerHTML = P;
							H[13].innerHTML = "";
							(T.channel == FileUtils.SHARE_CHANNEL_PUBLIC_LINK || T.channel == FileUtils.SHARE_CHANNEL_PUBLIC_SNS || T.channel == FileUtils.SHARE_CHANNEL_PRIVATE_LINK) ? H[13].innerHTML = "\u5206\u4eab\u7c7b\u578b:" + Y : H[14].innerHTML = Y;
							$(Z).attr({
								href : c,
								target : "_blank"
							});
							var b = H[12].innerHTML = K;
							b == "0" ? $(H[12]).parent().hide() : $(H[12]).parent().show();
							var W = H[15].innerHTML = O.length,
							S = $(H[15]).parent().parent();
							W == 0 ? S.hide() : S.show();
							for (var J = 0; J < W; J++) {
								A += "<span>" + O[J] + "</span>";
							}
							H[16].innerHTML = A;
							$(Z).prev().hide();
							$(".action-dd", Q).show();
							$(Z).removeClass("mis-examining");
						} else {
							$(Z).attr({
								href : c,
								target : "_blank"
							});
							$(".action-dd", Q).hide();
							$(Z).addClass("mis-examining");
							var D = G == 2 ? {
								clas : "mis-examining",
								html : "(\u5ba1\u6838\u4e2d)"
							}
							 : {
								clas : "mis-examined",
								html : "(\u5ba1\u6838\u5931\u8d25)"
							};
							$(Z).prev().show().attr("class", D.clas).html(D.html);
						}
						FileUtils.setInnerText(Z, f);
						T.property = -1;
						if (T.typicalCategory == -1) {
							T.isdir = 1;
						} else {
							T.category = T.typicalCategory;
						}
						if (T.fsIds.length > 1) {
							T.property = 2;
						}
						var R = null;
						if (FileUtils.isInParticularDir(FileUtils.parseDirPath(L))) {
							R = FileUtils.PART_KID_FOLDER;
							if (T.category == "3") {
								R = FileUtils.PART_KID_IMAGE;
							}
						}
						var E = FileUtils.resolveFileSmallIcon(T.isdir, T.fsIds, L.slice(L.lastIndexOf(".")), $(".b-ic-book", j), R, L);
						j.className = "inline-file-col";
						if (typeof(T.public) != "undefined" && T.public != 1) {
							$(j).find("em").addClass("share-type-private").attr("title", "\u79c1\u5bc6\u5206\u4eab");
						} else {
							$(j).find("em").removeClass("share-type-private").attr("title", "\u516c\u5f00\u5206\u4eab");
						}
						if (B._mPendingHighlightIndexes != null && B._mPendingHighlightIndexes[C] === true) {
							$(Q).addClass("highlight");
						} else {
							$(Q).removeClass("highlight");
						}
						return Q;
					};
				} else {
					return function (J, F, D) {
						var N = this._mElementsData[D],
						I = N.path,
						Q = I.lastIndexOf("/"),
						R = I.slice(0, I.lastIndexOf("/"));
						N.isdir = (N.isdir - 0);
						var H = (N.isdir ? "-" : FileUtils.toFriendlyFileSize(N.size)),
						V = Utilities.toDateString(N.server_mtime || N.ctime),
						W = FileUtils.getNormalizedPath(FileUtils.parseDirPath(I), true),
						A = W,
						C = document.body.className,
						U = _.call(this, J, F, K, W, I, D),
						K = U.view,
						S = U.txtHandler,
						P = U.pathHandler,
						X = U.iconSpan,
						T = U.checkbox,
						G = U.spans,
						Y = U.txtSpan,
						O = U.spanStart;
						$(P).attr("title", R ? R : "/\u5168\u90e8\u6587\u4ef6");
						FileUtils.setInnerText(P, R ? R.slice(R.lastIndexOf("/") + 1) : "\u5168\u90e8\u6587\u4ef6");
						var L = Page.inViewMode(Page.VIEW_RECYCLE_BIN);
						if (!L && (N.isdir || N.category == "4" || N.category == "2" || N.category == "3" || FileUtils.acceptOpenFileTypes(I.slice(I.lastIndexOf("."))))) {
							S.className = "file-handler b-no-ln dir-handler";
							S.style.cursor = "pointer";
						} else {
							if ((Page.inViewMode(Page.VIEW_HOME) || Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) && N.category == Category.MOV) {
								S.className = "file-handler b-no-ln dir-handler";
								S.style.cursor = "pointer";
							} else {
								S.className = "file-handler b-ig-ln";
								S.style.cursor = "default";
							}
						}
						if (!S.getAttribute("_installed")) {
							if (!L) {
								B._listenFolderOpenTriggers(S);
								B._listenFolderOpenTriggers(P);
								B._listenFolderOpenTriggers(X);
								B._listenDragFileTriggers(K);
								B._listenListViewItemDoubleClick(K);
							}
							B._listenClickTriggers(K);
							B._listenCheckboxTriggers(T);
							B._listenCheckboxDoubleClickTriggers(T);
							S.setAttribute("_installed", "1");
						}
						O += 2;
						G[O].innerHTML = H;
						$(G[O]).css("line-height", "37px");
						O++;
						G[O].innerHTML = V;
						$(G[O]).css("line-height", "37px");
						var M = null;
						if (FileUtils.isInParticularDir(FileUtils.parseDirPath(I))) {
							M = FileUtils.PART_KID_FOLDER;
							if (N.category == "3") {
								M = FileUtils.PART_KID_IMAGE;
							}
						}
						var E = FileUtils.resolveFileSmallIcon(N.isdir, N.fsIds, I.slice(I.lastIndexOf(".")), $(".b-ic-book", Y), M, I);
						Y.className = "inline-file-col";
						if (C.indexOf("flag-6") != -1) {
							if (typeof N.dateDiff == "undefined") {
								N.dateDiff = Utilities.dateDiff(N.server_mtime * 1000, FileUtils.systemTimestamp);
							}
							G[G.length - 1].innerHTML = N.dateDiff + "\u5929";
							$(G[G.length - 1]).css("line-height", "37px");
						}
						return K;
					};
				}
			})();
			this._mInfiniteListView.buildView = function (E, F, B) {
				var C = document.createElement("dd");
				C.className = "clearfix file-item";
				var A = ['<div class="file-col col">'],
				_ = document.body.className,
				D = false;
				if (Page.inViewMode(Page.VIEW_HOME)) {
					A.push('<span class="inline-commands b-btn clearfix"><div class="more-btn"><ul><li class="sell-sfile" title="\u51fa\u552e"><em class="icon-item-sellfile" _i="16"></em></li><li class="user-public" title="\u5206\u4eab"><em class="icon-share" _i="2"></em></li><li class="down-sfile" title="\u4e0b\u8f7d"><em class="icon-download" _i="8"></em></li><li class="more-sfile" title="\u66f4\u591a"><em class="icon-more-sfile"></em><div class="more-sfile-menu" title="\u66f4\u591a"><div class="more-sfile-inner"><div class="icon-move-sfile" _i="5" _data="move" title="\u79fb\u52a8\u5230">\u79fb\u52a8\u5230</div><div class="icon-copy-sfile" _i="4"  _data="copy" title="\u590d\u5236\u5230">\u590d\u5236\u5230</div><div class="icon-rename-sfile" _i="0" title="\u91cd\u547d\u540d">\u91cd\u547d\u540d</div><div class="icon-delete-sfile" _i="1" title="\u5220\u9664">\u5220\u9664</div></div></div></li></ul></div></span>');
				} else {
					if (_.indexOf("flag-2") != -1 && document.getElementById("barAllCmdTransfer")) {
						A.push('<span class="inline-commands b-btn clearfix"><div class="more-btn"><ul><li class="tomyrom-sfile" title="\u4fdd\u5b58\u81f3\u6211\u7684\u767e\u5ea6\u4e91"><em class="icon-tomyram" _i="1"></em></li><li class="down-sfile" title="\u4e0b\u8f7d"><em class="icon-download" _i="0"></em></li></ul></div></span>');
					} else {
						if (_.indexOf("flag-2") != -1 && document.getElementById("cancelButton")) {
							A.push('<span class="inline-commands b-btn clearfix"><div class="more-btn"><ul><li class="down-sfile" title="\u4e0b\u8f7d"><em class="icon-download" _i="0"></em></li></ul></div></span>');
						} else {
							if (_.indexOf("flag-4") != -1) {
								A.push('<span class="inline-commands b-btn clearfix"><div class="more-btn"><ul><li class="share-cancle" title="\u53d6\u6d88\u5206\u4eab"><em class="icon-share-cancle" _i="0"></em></li></ul></div></span>');
							} else {
								if (_.indexOf("flag-5") != -1) {
									A.push('<span class="inline-commands b-btn clearfix"></span>');
								} else {
									if (_.indexOf("flag-6") != -1) {
										A.push('<span class="inline-commands b-btn clearfix"><div class="more-btn"><ul><li class="reduction-sfile" title="\u8fd8\u539f"><em class="icon-reduction" _i="0"></em></li><li class="clear-sfile" title="\u5f7b\u5e95\u5220\u9664"><em class="icon-clear-file" _i="1"></em></li></ul></div></span>');
										D = true;
									} else {
										A.push('<span class="inline-commands b-btn clearfix"></span>');
									}
								}
							}
						}
					}
				}
				if (_.indexOf("flag-4") != -1 || _.indexOf("flag-10") != -1) {
					A.push('<span class="inline-file-col"><a hidefocus="true" unselectable="on" href="javascript:;" class="b-in-blk input-cbx"><dfn><s class="sprite"></s></dfn></a><em class="b-in-blk share-type-ic" title="\u5206\u4eab\u7c7b\u578b"></em><span class="b-in-blk sprite-list-ic b-ic-book"></span><s></s><a hidefocus="true" unselectable="on" class="file-handler b-no-ln dir-handler" href="javascript:;"></a></span>');
					A.push('</div><div class="time-col col"><span></span></div><div class="size-col col"><span></span></div>');
					D = true;
				} else {
					A.push('<span class="inline-file-col"><a hidefocus="true" unselectable="on" href="javascript:;" class="b-in-blk input-cbx"><dfn><s class="sprite"></s></dfn></a><span class="b-in-blk sprite-list-ic b-ic-book"><s></s></span><a hidefocus="true" unselectable="on" class="file-handler b-no-ln dir-handler" href="javascript:;"></a></span>');
					A.push('</div><div class="size-col col"><span></span></div><div class="time-col col"><span></span></div><div class="pathing-col col"><a hidefocus="true" unselectable="on" class="path-handler b-no-ln dir-handler" href="javascript:;"></a></div>');
				}
				if (D) {
					if (_.indexOf("flag-4") != -1 || _.indexOf("flag-10") != -1) {
						A.push('<div class="remaining-col col"><span>-</span></div><div class="remaining-col col"><span class="d-cnt">-</span><span class="b-in-blk sprite-ic action-dd"></span></div><div class="share-warn"><div class="share-warn-tit"><div class="share-copy-outer"><span class="sl-tit">\u94fe\u63a5\uff1a</span><span class="sl-url">-</span><span class="sl-pwd"><span class="sl-pwd-tit">\u63d0\u53d6\u5bc6\u7801\uff1a</span><span class="sl-yzm">-</span></span><span class="share-style"></span><a href="javascript:;" hidefocus="true" class="bbtn share_copy_btn" _i="0"><em class="icon-copy"></em><b>\u590d\u5236</b></a></div></div><div class="share-warn-cnt"><div class="sl-fr">\u79c1\u5bc6\u5206\u4eab\uff1a<span class="share-style2"></span>(<span>0</span>\u4e2a)</div><div class="sl-fr-rul"><span></span></div></div></div>');
					} else {
						A.push('<div class="remaining-col col"><span>-</span></div>');
					}
				}
				C.innerHTML = A.join("");
				return C;
			};
			this._mInfiniteListView.getIdForPosition = function (_) {
				return this._mElementsData[_].fs_id;
			};
			this._mInfiniteListView.lookupPositionForId = function (C) {
				var A = this._mElementsData;
				for (var B = 0, _ = A.length; B < _; B++) {
					if (A[B].fs_id == C) {
						return B;
					}
				}
				return -1;
			};
			this._mInfiniteListView.isFirstCheckedChildVisible = function () {
				for (var A = 0, _ = this._mCheckedChildren.length; A < _; A++) {
					if (this.isItemChecked(A)) {
						return A >= this._mFirstPosition && A < this._mFirstPosition + this._mChildren.length;
					}
				}
				return false;
			};
			if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				this._mInfiniteListView.onScroll = function (A, _) {
					var B = this;
					if (FileUtils._mDropdownDrawerFlag == 1) {
						FileUtils._shareDropdownDrawerTrigger(null, true, 200);
						setTimeout(function () {
							B.onScroll(A, _);
						}, 400);
						return;
					}
					disk.ui.RecycleListView.prototype.onScroll.call(this, A, _);
				};
			}
			this._mInfiniteListView.onComputeScrollbarState = function (D, A, _) {
				disk.ui.RecycleListView.prototype.onComputeScrollbarState.call(this, D, A, _);
				if (A > 0 && this.getCount() > 0 && !B._mQuerying) {
					var C = parseFloat((A + _) / this.getCount(), 10);
					if (C > 0.7) {
						var E = null;
						if (FileUtils.inSearchMode()) {}
						else {
							if (B._mModule == "category") {
								E = B._mCategoryProxyLoader;
							} else {
								E = B._mProxyLoader;
							}
						}
						if (E && !E.isIdle()) {
							E.load();
						}
						if (E) {
							E.endShowMsg(true);
						}
					}
				}
				if (typeof FileUtils._whenListScrolledCallBack == "function") {
					FileUtils._whenListScrolledCallBack();
				}
			};
			this._mInfiniteListView.onSystemNotify = this._onSystemNotify;
			this._mInfiniteListView.activateKeyguard(true);
			this._mInfiniteListView.getScrollbar(true).awake(true);
			if (A === true) {
				this.importCheckedStateBetweenViews(true);
			}
			this._mInfiniteListView.setBackedData(_);
		} else {
			this._mInfiniteListView.lock(false, true);
			this._mInfiniteListView.awakeScrollbar(true);
			if (A === true) {
				this.importCheckedStateBetweenViews(true);
				this._mInfiniteListView.setBooleanFlag(disk.ui.RecycleListView.PRESERVE_CHECKED_STATE, true);
			}
			this._mInfiniteListView.changeBackedData(_);
			if (A === true) {
				this._mInfiniteListView.setBooleanFlag(disk.ui.RecycleListView.PRESERVE_CHECKED_STATE, false);
			}
		}
	},
	_onSystemNotify : function (C, F, B) {
		switch (C) {
		case disk.ui.RecycleListView.NOTIFY_SYSTEM_LOCK:
			if (F) {
				$("#panInner").addClass("lock");
			} else {
				$("#panInner").removeClass("lock");
			}
			Page.obtain().resolveToolbarMenu(F);
			break;
		case disk.ui.RecycleListView.NOTIFY_LIST_EMPTY:
			if (F && $("#fileList .no-result-title").length > 0) {
				return;
			}
			$("#inifiniteListViewEmptyNote").html(B === true ? "\u6b63\u5728\u52a0\u8f7d\u6570\u636e\uff0c\u8bf7\u7a0d\u5019&hellip;" : "\u6ca1\u6709\u6570\u636e");
			$("#inifiniteListViewEmptyNote").css("display", F ? "block" : "none");
			if (F) {
				$(".files-header").hide();
				if (FileUtils._mViewMode == FileUtils.VIEW_GRID_ICON) {
					$("#fileThumbHeader").show();
				} else {
					if (FileUtils.inSearchMode()) {
						$("#sortColsHeader").show();
						$(".all-files-headers,#infiniteListView").addClass("searchMode");
					} else {
						$("#sortColsHeader").show();
						$(".all-files-headers,#infiniteListView").removeClass("searchMode");
					}
				}
			}
			break;
		case disk.ui.RecycleListView.NOTIFY_LIST_REPAINT:
			if (disk.DEBUG) {
				console.log("List Repainting");
			}
			var E = (FileUtils.getViewMode() == FileUtils.VIEW_LIST) ? this.getCount() : this.getCountEx(),
			D = null,
			_ = null,
			A = null;
			if (FileUtils.inSearchMode()) {}
			else {
				if (FileUtils.getModule() == "category") {
					D = FileUtils.getCategoryProxyLoader();
				} else {
					D = FileUtils.getProxyLoader();
					if (D != null) {
						A = D.getTag();
					}
				}
			}
			if (A != null && A != FileUtils.resolveCurrentDirKey()) {
				D = null;
			}
			if (D != null && D.hasMorePage()) {
				_ = E == 0 ? "\u6b63\u5728\u52a0\u8f7d\u6587\u4ef6hellip;" : "\u5df2\u52a0\u8f7d" + E + "\u4e2a";
				$("#dirData .loadedDate").html("\u5df2\u52a0\u8f7d" + E + "\u4e2a");
			} else {
				if (D != null && !D.hasMorePage()) {
					_ = E == 0 ? "\u7a7a\u6587\u4ef6\u5939" : "\u5217\u8868\u52a0\u8f7d\u5b8c\u6bd5\uff0c\u5171" + E + "\u4e2a";
					$("#dirData .loadedDate").html("\u5df2\u5168\u90e8\u52a0\u8f7d\uff0c\u5171" + E + "\u4e2a");
				} else {
					_ = E == 0 ? "\u6b63\u5728\u52a0\u8f7d\u6587\u4ef6hellip;" : "\u5217\u8868\u52a0\u8f7d\u5b8c\u6bd5\uff0c\u5171" + E + "\u4e2a";
					$("#dirData .loadedDate").html("\u5df2\u5168\u90e8\u52a0\u8f7d\uff0c\u5171" + E + "\u4e2a");
				}
			}
			if ($(".files-header").hasClass("on")) {
				$("#file_action_word").text("\u5df2\u9009\u4e2d" + E + (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) ? "\u4e2a\u5916\u94fe" : "\u4e2a\u6587\u4ef6/\u6587\u4ef6\u5939"));
			}
			break;
		}
	},
	initIntr : function (A, _, B) {
		return;
		if (B < 3) {
			if (disk.DEBUG) {
				console.log("Error on InitIntr: all < 3 !");
			}
			return;
		}
		if (A == 0) {
			this.buildIntr(B, 1);
			return;
		}
	},
	buildIntr : function (A, _) {},
	load : function (F, B) {
		this.deactivateListView(false);
		var E = this,
		A = this._mLocalCache.obtain(F),
		C = this._mPendingHighlights != null;
		if (C) {
			this._translatePendingFilesIndexes(A);
		}
		if (!FileUtils._mFileListDOM) {
			FileUtils._mFileListDOM = document.getElementById("infiniteListView");
			this._mSelectionArbitrate = $(".files-header");
			$(".selectionArbitrate", this._mSelectionArbitrate).click(function (B) {
				if (E.isListViewLocked()) {
					return;
				}
				$(".files-header").toggleClass("on");
				var _ = $(".files-header").hasClass("on"),
				A = E._mInfiniteListView;
				if (A) {
					A.setItemsChecked(_);
				}
				if (E._mInfiniteGridView) {
					E._mInfiniteGridView.setItemsChecked(_);
				}
				FileUtils.repaintListView();
				E.changeProperty();
				Page.obtain().resolveToolbarMenu();
				return false;
			});
		}
		if (!FileUtils._mFileGridDOM) {
			FileUtils._mFileGridDOM = document.getElementById("infiniteGridView");
		}
		if (this._mViewMode == FileUtils.VIEW_LIST) {
			if (disk.DEBUG) {
				console.log("Switch to list mode, last view mode is ", this._mLastViewMode);
			}
			this._onLoadListView(A, B);
		} else {
			if (disk.DEBUG) {
				console.log("Switch to grid mode, last view mode is ", this._mLastViewMode);
			}
			var _ = this.setGroupData(A, this._mGridStep);
			this._onLoadGridView(_, B);
		}
		if (C) {
			if (disk.DEBUG) {
				console.log("[LOG]#####has to test highlight, too bad,===> ", this._mPendingHighlightFirst);
			}
			$(FileUtils._mFileListDOM).addClass("highlight-on");
			$(FileUtils._mFileGridDOM).addClass("highlight-on");
			this.scheduleHidingHighlight();
			if (this._mViewMode == FileUtils.VIEW_LIST) {
				if (this._mPendingHighlightFirst != -1 && !this._mInfiniteListView.hitInVisibleRegion(this._mPendingHighlightFirst)) {
					if (disk.DEBUG) {
						console.log("[LOG]first highlighted item is not in visible region, we has to scroll");
					}
					var D = Math.min(Math.max(1, this._mPendingHighlightFirst + 1), this._mInfiniteListView.getCount() - 1);
					this._mInfiniteListView.scrollToPosition(D);
				}
			} else {
				if (this._mPendingHighlightFirst != -1) {
					this._mInfiniteGridView.scrollToPositionEx(this._mPendingHighlightFirst + 1);
				}
			}
		}
		this._mPendingHighlights = null;
		this._mPendingHighlightIndexes = null;
		this._mPendingHighlightFirst = -1;
		this._afterListLoad(A);
		Page.obtain().resolveToolbarMenu();
		unify.st.Bootstrap.getPageWindow().sendListViewReady();
	},
	changeProperty : function (_) {},
	childProperty : function (C) {
		var _ = 0,
		B = C.list.length,
		A = $("#property-share-p");
		A.html("");
		while (_ < B) {
			A.append('<span class="property-share-p">' + C.list[_] + "</span>");
			_++;
			if (_ > 4) {
				A.append('<span class="property-share-p">\u2026\u2026</span>');
				break;
			}
		}
	},
	historyNavigate : function (_) {
		return disk.util.HistoryManager.getDefault().navigate(FileUtils.MODULE_DIR, _);
	},
	_listenMouseMovement : function (_) {
		_.onmouseover = function () {
			if ((FileUtils._mInfiniteListView != null && !FileUtils._mInfiniteListView.locked()) || (FileUtils._mInfiniteGridView != null && !FileUtils._mInfiniteGridView.locked())) {
				var C = $(this);
				if ($(window).height() - C.offset().top < 145) {
					C.addClass("hover hover-low");
				} else {
					C.addClass("hover");
				}
				var B = $(".inline-commands", this);
				B.css("visibility", "visible");
				var _ = FileUtils.getElementData(C.attr("_position")),
				A = FileUtils._checkIsBuyFile(_ && _.path || "");
				if (A || (_ && _.isdir == 1)) {
					B.find(".sell-sfile").hide();
				} else {
					B.find(".sell-sfile").hide();
				}
			}
		};
		_.onmouseout = function (_) {
			$(this).removeClass("hover hover-low");
			$(".inline-commands", this).css("visibility", "hidden");
			$(".more-sfile-menu", this).hover(function () {
				$(this).show();
			}, function () {
				$(this).hide();
			});
		};
	},
	_whenListScrolledCallBack : null,
	_afterListDoWizard : function (B, _, A) {
		disk.Context.requestFeature(disk.Context.FEATURE_STORAGE, {
			succ : function () {
				var H = localStorage.getItem("flag_show_functions_" + _);
				if (disk.DEBUG) {
					console.warn("showWizard: ", H, " display: ", $("#infiniteListView").css("display"), B);
				}
				if (H == 1 || $("#infiniteListView").css("display") == "none" || (B && B.length == 0)) {
					$("#wizard-functions-intro").remove();
					return;
				}
				var J,
				M,
				D = -1,
				K = $("#infiniteListView .file-item").size();
				if (disk.DEBUG) {
					console.warn("size: ", K);
				}
				for (var E = 0; E < K; E++) {
					J = B[E];
					if ((!J.isdir) && J.server_filename) {
						M = J.server_filename.match(/(.[^\.]+$)/)[1];
						if (M && _.indexOf(M.toLowerCase() + "_") >= 0) {
							D = E;
							break;
						}
					}
				}
				if (disk.DEBUG) {
					console.warn("resultIndex: ", D);
				}
				if (D == -1) {
					$("#wizard-functions-intro").remove();
					return;
				}
				var C = $("#infiniteListView .file-item[_position='" + D + "']"),
				G,
				F,
				L;
				if (C && C.length > 0) {
					L = C.offset();
					G = L.left;
					F = L.top;
				} else {
					return;
				}
				localStorage.setItem("flag_show_functions_" + _, "1");
				setTimeout(function () {
					FileUtils._whenListScrolledCallBack = function () {
						$("#wizard-functions-intro").remove();
						FileUtils._whenListScrolledCallBack = null;
					};
				}, 2000);
				var I = $("#wizard-functions-intro");
				if (I.length > 0) {}
				else {
					I = $('<div id="wizard-functions-intro"><div class="intro-inner">' + A + '<a class="intro-close" hidefocus="true" href="javascript:;"></a></div><div class="intro-bg"></div></div>');
					$(document.body).append(I);
				}
				I.css({
					left : G,
					top : F - 145
				});
				I.find(".intro-close").one("click", function () {
					I.remove();
				});
			}
		});
	},
	_afterListLoad : function (B) {
		if ($("#fileList .no-result-title").length > 0) {
			return;
		}
		var A = false;
		if (B && B.length === 0) {
			if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
				$("#inifiniteListViewEmptyNote").html('<div class="no-result-div"><p class="no-result-pic"><em class="recy"></em></p><p class="no-result-title">\u56de\u6536\u7ad9\u4e3a\u4f60\u4fdd\u755910\u5929\u5185\u5220\u9664\u7684\u6587\u4ef6\u3002</p></div>');
			} else {
				if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
					$("#inifiniteListViewEmptyNote").html('<div class="no-result-div"><p class="no-result-pic"><em class="shr"></em></p><p class="no-result-title">\u597d\u4e1c\u897f\u8981\u6652\u51fa\u6765\uff0c\u5feb\u628a\u4f60\u7684\u6587\u4ef6\u5206\u4eab\u7ed9\u8eab\u8fb9\u7684\u670b\u53cb\u4eec\u5427\u3002</p></div>');
				} else {
					if (Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
						$("#inifiniteListViewEmptyNote").html('<div class="no-result-div"><p class="no-result-pic"><em class="shr-home-slave"></em></p><p class="no-result-title">Ta\u8fd8\u6ca1\u6709\u5206\u4eab\u5462\uff0c\u8fc7\u6bb5\u65f6\u95f4\u518d\u6765\u770b\u770b\u5427~</p></div>');
					} else {
						if (Page.inViewMode(Page.VIEW_HOME)) {
							if (FileUtils.getModule() === "category" && FileUtils.getType() === 2) {
								if (FileUtils.getFlagTimeLineByCookie()) {
									this.makeTimeLineEmptyHtml();
								} else {
									this.makeNewEmptyHTML();
								}
								if ($("#timeLineGuide").length > 0) {
									$("#timeLineGuide").remove();
								}
							} else {
								if (FileUtils.getModule() === "category" && FileUtils.getType() == 3) {
									$("#inifiniteListViewEmptyNote").html('<div class="no-result-file"><p class="no-result-video"><em class="nopic"></em></p><p class="no-result-title video"><span></span></p></div>');
									var _ = $("#inifiniteListViewEmptyNote .no-result-title");
									_.addClass("not-installed");
									_.find("span").append($('<a href="http://dl.client.baidu.com/BaiduPlayer/BaiduPlayerWithCloudDisk.exe">\u4e0b\u8f7d\u89c6\u9891\u64ad\u653e\u63d2\u4ef6</a>'));
								} else {
									if (this._mDirMgr._mStack[0] == "\u6765\u81eaPC\u7684\u5907\u4efd\u6587\u4ef6") {
										var C = "http://pan.baidu.com/download#pan";
										$("#inifiniteListViewEmptyNote").html('<div class="no-result-file"><p class="no-result-PC"><em class="nofile"></em><a target="_blank" class="lbtn icon-pl35 windowsIcon" href="' + C + '"><em class="icon-window"></em><b>\u4e0b\u8f7dWindows\u7248</b></a></p><p class="no-result-title"> </p></div>');
									} else {
										if (FileUtils.getModule() === "category" && FileUtils.getType() == 7) {
											this.makeNewEmptyHTMLByBT();
										} else {
											this.makeNewEmptyHTML();
										}
									}
								}
							}
						} else {}

					}
				}
			}
			A = true;
		}
		if (Page.inViewMode(Page.VIEW_RECYCLE_BIN) && B == null) {
			A = true;
			$("#inifiniteListViewEmptyNote").html('<div class="no-result-div"><p class="no-result-pic"><em class="recy"></em></p><p class="no-result-title">\u56de\u6536\u7ad9\u4e3a\u4f60\u4fdd\u755910\u5929\u5185\u5220\u9664\u7684\u6587\u4ef6\u3002</p></div>');
		}
		if (typeof disk.ui.MouseWheelSensor != "undefined") {
			disk.ui.MouseWheelSensor.FLAG_PREVENT_DEFAULT = !A;
		}
		if (disk.DEBUG) {
			console.warn("resolveCurrentDirKey: ", FileUtils.resolveCurrentDirKey(), ", FLAG_COMING_BY_SEARCH:", FileUtils.FLAG_COMING_BY_SEARCH);
		}
		if (Page.inViewMode(Page.VIEW_HOME) && (FileUtils.resolveCurrentDirKey() != 0)) {
			if (FileUtils.FLAG_COMING_BY_SEARCH == 1) {
				FileUtils.FLAG_COMING_BY_SEARCH = 0;
			} else {
				this._afterListDoWizard(B, ".zip_.rar_", "\u5355\u51fb\u5373\u53ef\u67e5\u770b\u538b\u7f29\u5305\uff0c</br>\u514d\u53bb\u4e0b\u8f7d\u70e6\u607c\uff01");
			}
		}
	},
	_mHotBtRecList : null,
	makeNewEmptyHTMLByBT : function () {
		FileUtils.loginState = "1";
		var _ = $("#inifiniteListViewEmptyNote"),
		B = function () {
			var A = ['<div class="no-result-file-download">', '<div class="no-result-title"><div>', '<div class="empty-bt clearfix"><div class="empty-bt-pic"></div><div class="empty-bt-txt">\u4f60\u8fd8\u6ca1\u6709\u79cd\u5b50\u6587\u4ef6\u5462,\u53bb\u770b\u770b\u522b\u4eba\u5206\u4eab\u7684\u70ed\u95e8\u79cd\u5b50\u5427</div></div>', '<div class="empty-bt-content-list"></div>', "</div>"];
			_.html(A.join(""));
		},
		A = function () {
			var A = FileUtils._mRecommendBtPanel;
			A.startInit(_.find(".empty-bt-content-list"));
			FileUtils._mDiskLog.send({
				type : "hotrec_bt_empty_display"
			});
		};
		B();
		return;
		if (FileUtils._mRecommendBtPanel == null) {
			if (typeof disk.ui.ReCommendBT == "undefined") {
				ark.use({
					url : ["/buss/recommend_bt_panel", "/recommend_bt.css"],
					noDependency : true
				}, function () {
					if (typeof disk.ui.ReCommendBT != "undefined") {
						FileUtils._mRecommendBtPanel = disk.ui.ReCommendBT.obtain();
						A();
					}
				});
			} else {
				FileUtils._mRecommendBtPanel = disk.ui.ReCommendBT.obtain();
				A();
			}
		} else {
			A();
		}
	},
	makeTimeLineEmptyHtml : function () {
		var A = $("#inifiniteListViewEmptyNote"),
		_ = ['<div style="margin-left:0" class="no-result-file-download">', '<div class="no-result-title"><div class="no-result-timeline"></div></div>', "</div>"];
		A.html(_.join(""));
	},
	makeNewEmptyHTML : function () {
		var B = false;
		if (FileUtils._mDataAwardIntrCache == 0) {
			B = true;
		}
		var _ = '<div class="no-result-file-download">' + '<div class="' + (B ? "no-result-file-noupload-caption" : "no-result-file-caption") + '"></div>' + (B ? '<a target="_blank" class="no-result-file-noupload-award" href="/disk/award" hidefocus="true">\u53bb\u6269\u5bb9 &gt;</a>' : "") + '<div class="no-result-title no-result-file-bottom">' + '<a hidefocus="true" class="no-result-download-phone" href="http://pan.baidu.com/download#android" target="_blank">\u624b\u673a\u7248\u4e0b\u8f7d</a>' + '<a hidefocus="true" class="no-result-download-twocode" href="javascript:;">\u4e8c\u7ef4\u7801\u4e0b\u8f7d</a>' + '<a hidefocus="true" class="no-result-download-ipad" href="http://pan.baidu.com/download#ipad" target="_blank">iPad\u7248\u4e0b\u8f7d</a>' + '<a hidefocus="true" class="no-result-download-pc" href="http://pan.baidu.com/download" target="_blank">PC\u7248\u4e0b\u8f7d</a>' + '<div class="no-result-file-twocode"><a hidefocus="true" href="javascript:;">\u5173\u95ed</a></div>' + "</div>" + "</div>",
		D = $("#inifiniteListViewEmptyNote"),
		C,
		A;
		D.html(_);
		C = D.find(".no-result-file-twocode");
		A = C.find("a");
		D.find(".no-result-download-twocode").mouseover(function () {
			C.show();
		}).mouseout(function () {
			C.hide();
		});
		C.mouseover(function () {
			C.show();
		}).mouseout(function () {
			C.hide();
		});
		A.click(function () {
			C.hide();
		});
		if (FileUtils._mDataAwardIntrCache == null) {
			$.ajax({
				type : "POST",
				timeout : 3000,
				url : disk.api.RestAPI.TASK_QUERY + "?detail=1&t=" + (new Date().getTime()),
				dataType : "json",
				success : function (A) {
					if (A && A.errno == 0) {
						if (A.task_detail && A.task_detail[6] == 0) {
							FileUtils._mDataAwardIntrCache = 0;
							var _ = $("#inifiniteListViewEmptyNote").find(".no-result-file-caption");
							if (_.length) {
								_.addClass("no-result-file-noupload-caption");
								$('<a target="_blank" class="no-result-file-noupload-award" href="/disk/award" hidefocus="true">\u53bb\u6269\u5bb9 &gt;</a>').insertAfter(_);
							}
						}
					}
				}
			});
		}
	},
	bindDownloadClient : function () {
		$(".side-options a").mousedown(function () {
			var _ = "",
			A = "sidebar";
			if ($(this).hasClass("side-lwindows")) {
				_ = "win";
			} else {
				if ($(this).hasClass("side-landroid")) {
					_ = "android";
				} else {
					if ($(this).hasClass("side-liphone")) {
						_ = "iphone";
					} else {
						if ($(this).hasClass("side-lipad")) {
							_ = "ipad";
						} else {
							if ($(this).hasClass("side-lwp7")) {
								_ = "wp7";
							} else {
								return;
							}
						}
					}
				}
			}
			setTimeout(function () {
				FileUtils._mDiskLog.send({
					type : "downloadclient",
					ttype : _,
					pos : A
				});
			}, 500);
			return true;
		});
	},
	bindCategoryClickLog : function (_) {
		FileUtils._mDiskLog.send({
			type : "bindCategoryClickLog",
			category : _
		});
		return true;
	},
	clearAllDocData : function () {
		if (typeof disk.ui.DocReader == "undefined") {
			ark.use({
				url : ["/buss/dialog/docreader_dialog.js", "/dialog/doc-dialog.css"],
				noDependency : true
			}, function () {
				if (typeof disk.ui.DocReader != "undefined") {
					disk.ui.DocReader.clearAllDocData();
				} else {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u6587\u4ef6\u5220\u9664\u51fa\u9519\uff0c\u8bf7\u5237\u65b0\u9875\u9762\u91cd\u8bd5",
						sticky : false
					});
				}
			});
		}
	},
	operateCachePath : function (C, B) {
		if ((typeof localStorage === "undefined") || (!window.FileUtils) || (!window.FileUtils.sysUID) || (typeof localStorage.getItem === "undefined")) {
			return false;
		}
		var _ = "cachepath_" + FileUtils.sysUID.replace(/@/g, ""),
		A = "";
		if (C) {
			if (disk.DEBUG) {
				console.log(">>> cache path: set key=", _, " value=", B);
			}
			localStorage.setItem(_, B);
			return true;
		} else {
			A = localStorage.getItem(_);
		}
		if (A !== null) {
			if (disk.DEBUG) {
				console.log(">>> cache path: get key=", _, " value=", A);
			}
			return A;
		} else {
			return false;
		}
	}
}, 
Page = function () {
	this._mUploadDialog = null;
	this._mMoveCopyDialog = null;
	this._mPublicDialog = null;
	this._mShareDialog = null;
	this._mSingleURLDialog = null;
	this._mToolbarMenuItems = [];
	this._mBarCmdTrigger = null;
};
Page.singleton = null;
Page.obtain = function () {
	return Page.singleton ? Page.singleton : (Page.singleton = new Page());
};
Page.SHARE = 1;
Page.PUBLIC = 2;
Page.MAX_FILE_DELECTION_ALLOWED = 100;
Page.MENU_RENAME = 0;
Page.MENU_DOWNLOAD = 0;
Page.MENU_TRANSFER = 1;
Page.MENU_ALLFILEDOWNLOAD = 2;
Page.MENU_ALLFILETRANSFER = 3;
Page.MENU_CANCEL_SHARE = 0;
Page.MENU_CANCEL_PUBLIC = 0;
Page.MENU_DELETE = 1;
Page.MENU_SHARE = 2;
Page.MENU_PUBLIC = 3;
Page.MENU_COPY = 4;
Page.MENU_MOVE = 5;
Page.MENU_UPLOAD = 6;
Page.MENU_NEW_FOLDER = 7;
Page.MENU_ANOTHER_DOWNLOAD = 8;
Page.MENU_PLAY_ALL = 9;
Page.MENU_NEW_OFFLINE = 10;
Page.MENU_OFFLINE_LIST = 11;
Page.MENU_NEW_BT_OFFLINE = 12;
Page.MENU_EDIT_PIC = 13;
Page.MENU_PRINT = 14;
Page.MENU_REVISION_LIST = 15;
Page.MENU_SELL_FILE = 16;
Page.MENU_BAR_SELL_FILE = 17;
Page.MENU_SELL_FILE_BIG = 18;
Page.MENU_DEVICE = 19;
Page.MENU_DEVICE_OPEN = 20;
Page.MENU_DEVICE = 19;
Page.MENU_RESTORE = 0;
Page.MENU_PERSIST_REMOVE = 1;
Page.MENU_CLEAR_ALL = 2;
Page.VIEW_HOME = 0;
Page.VIEW_SHARE_PROPERTY_OWN = 4;
Page.VIEW_PROPERTY_OTHER = 2;
Page.VIEW_PROPERTY_NOTFOUND = 8;
Page.VIEW_HOME_PAGE_SLAVE = 10;
Page.VIEW_RECYCLE_BIN = 6;
Page.inViewMode = function (_) {
	var A = document.body.className;
	if (typeof _ == "undefined") {
		return A.substring(5);
	}
	switch (_) {
	case Page.VIEW_HOME_PAGE_SLAVE:
		return A.indexOf("flag-10") != -1;
		break;
	case Page.VIEW_HOME:
		return A.indexOf("flag-10") == -1 && A.indexOf("flag-1") != -1;
		break;
	case Page.VIEW_SHARE_PROPERTY_OWN:
		return A.indexOf("flag-4") != -1;
		break;
	case Page.VIEW_PROPERTY_OTHER:
		return A.indexOf("flag-2") != -1;
		break;
	case Page.VIEW_RECYCLE_BIN:
		return A.indexOf("flag-6") != -1;
		break;
	}
};
Page.createAwardBubble = function (B, _) {
	return;
	if (B) {
		if (_ != 0) {
			return;
		} else {
			_this = this;
			if ($("#bubbleAwardPanel").length == 0) {
				var A = $("<div>", {
						"class" : "bubble-panel",
						id : "bubbleAwardPanel"
					}).html('<div class="bubble-arrow reverse"></div><div class="bubble-panel-main" id="bubbleMainContent">\u9080\u8bf7\u670b\u53cb\uff0c\u83b7\u5f97\u5927\u989d\u7a7a\u95f4\u5956\u52b1\u3002<a class="b-in-blk bubble-close" href="javascript:;"></a></div>').hide();
				A.appendTo("#awardBubbleP");
				$(".bubble-close").click(function (_) {
					_this.createAwardBubble(false);
				});
				$("#bubbleAwardPanel").fadeIn("slow");
			}
		}
	} else {
		$("#bubbleAwardPanel").hide("fast", function () {
			$("#bubbleAwardPanel").detach();
		});
	}
};
Page.setCookie = function (G, _, D, E, C) {
	var A = new Date(),
	B = "",
	F = "";
	A.setDate(A.getDate() + D);
	if (C) {
		B = ";domain=" + C;
	}
	if (E) {
		F = ";path=" + E;
	}
	document.cookie = G + "=" + escape(_) + ((D == null) ? "" : ";expires=" + A.toGMTString()) + F + B;
};
Page.getCookie = function (_) {
	if (document.cookie.length > 0) {
		c_start = document.cookie.indexOf(_ + "=");
		if (c_start != -1) {
			c_start = c_start + _.length + 1;
			c_end = document.cookie.indexOf(";", c_start);
			if (c_end == -1) {
				c_end = document.cookie.length;
			}
			return unescape(document.cookie.substring(c_start, c_end));
		}
	}
	return "";
};
Page.obtainDownloadURL = function (C) {
	var B = disk.util,
	A = "";
	if (B && B.PCSUtils && B.PCSUtils._sResult) {
		A = "&cflg=" + encodeURIComponent(B.PCSUtils._sResult + ":" + B.PCSUtils._sRevision);
	}
	if (disk.DEBUG) {
		console.log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>pcs suffix ", A);
	}
	return ($.browser.msie) ? C.dlink + "&response-cache-control=private" + A : C.dlink + A;
	if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
		return C.dlink;
	} else {
		var E = C.path,
		D = FileUtils.parseDirFromPath(E);
		if (!C.isdir) {
			var _ = D.lastIndexOf(".");
			if (_ != -1) {
				D = D.substring(0, _);
			}
		}
		return disk.api.RestAPI.SINGLE_DOWNLOAD + "&path=" + encodeURIComponent(FileUtils.parseDirPath(E));
	}
};
Page.showNotification = function (D, B, C) {
	var _ = document.getElementById("netdiskTips"),
	E = document.getElementById("subwindowContainer"),
	A = document.getElementById("panInner");
	if (!E || !A) {
		if (disk.DEBUG) {
			console.warn("could not locate an spot for notification tips");
		}
		return null;
	} else {
		if (D) {
			if (!_) {
				_ = document.createElement("div");
				_.className = "netdiskTips";
			} else {
				if (_.getAttribute("_installed") && C !== true) {
					if (disk.DEBUG) {
						console.warn("an existing notification showed, duplicate request rejected");
					}
					return;
				}
			}
			_.setAttribute("_installed", 1);
			_.innerHTML = '<div class="close-tips" title="\u5173\u95ed\u63d0\u793a"></div><div class="tips ellipsis clearfix"><em class="sprite-ic tips-prx"></em>' + B + "</div>";
			_.style.position = "relative";
			_.style.zIndex = "10";
			_.style.display = "block";
			E.insertBefore(_, A);
		} else {
			if (_) {
				_.style.display = "none";
			}
		}
		Utilities.resolveLayout();
		return _;
	}
};
Page.changeUnionLogin = function () {
	var B = "frm",
	G = disk.getParam(B);
	if (G) {
		if (G == "hao123") {
			var F = $("#header .logo"),
			A = F.clone(false);
			A.css({
				backgroundPosition : "0 -160px",
				width : 100,
				outline : "none"
			}).attr({
				title : "hao123\u4e0a\u7f51\u5bfc\u822a",
				href : "http://hao123.com/",
				target : "_blank",
				hidefocus : "true"
			}).addClass("logo-hao123");
			F.css({
				backgroundPosition : "-101px -160px",
				width : 96,
				marginLeft : 0,
				outline : "none"
			}).attr({
				title : "\u767e\u5ea6\u4e91",
				href : "http://yun.baidu.com/",
				target : "_blank",
				hidefocus : "true"
			}).addClass("logo-yun");
			F.before(A);
			var E = "?" + B + "=" + G,
			D = function (_) {
				var A = _,
				B = A.attr("href"),
				C = B.split("#");
				if (B && B != "javascript:;") {
					if (C.length > 1) {
						A.attr("href", C[0] + E + "#" + C[1]);
					} else {
						A.attr("href", B + E);
					}
				}
			};
			$("#aside-menu").find("a").each(function () {
				D($(this));
			});
			var H = $(".logo-hao123", "#header").innerWidth() + $(".logo-yun", "#header").innerWidth();
			if (H + $(".navs", "#header").innerWidth() + $(".info>ul", "#header").innerWidth() + 30 > $("#header").width()) {
				$(".logo-hao123", "#header").hide().next().css("marginLeft", 10);
			}
		} else {
			if (G == "hao123-iframe") {
				$(".module-header").parent("div").hide();
				E = "?" + B + "=" + G,
				D = function (_) {
					var A = _,
					B = A.attr("href"),
					C = B.split("#");
					if (B && B != "javascript:;") {
						if (C.length > 1) {
							A.attr("href", C[0] + E + "#" + C[1]);
						} else {
							A.attr("href", B + E);
						}
					}
				};
				$("#aside-menu").find("a").each(function () {
					D($(this));
				});
				var C = $(".sharenum a");
				for (var _ = 0; _ < C.length; _++) {
					C.eq(_).attr("href", C.eq(_).attr("href") + "&frm=hao123-iframe");
				}
				Utilities.resolveLayout();
			}
		}
	}
};
Page.prototype = {
	resolveToolbarUI : function (_, A) {
		var B = $(".process", _);
		if ("number" == typeof A || A instanceof Number) {
			if (B.css("display") == "none") {
				return null;
			}
			A ? B.css({
				width : A + 1 + "px",
				paddingRight : "1px"
			}) : B.css({
				width : "0px",
				paddingRight : "0px"
			});
		} else {
			if ("string" == typeof A || A instanceof String) {
				$(".text", _).text(A);
			} else {
				if ("boolean" == typeof A || A instanceof Boolean) {
					A ? B.removeClass("none") : B.addClass("none");
				} else {
					if (typeof A == "undefined") {
						return B.css("display") == "none" ? undefined : B.width() / (B.parent().parent().width() - 2) * 100;
					}
				}
			}
		}
	},
	dispatchFileSelectionChange : function (D, F, C, _, G, A) {
		return false;
		var B = false,
		E = null;
		if (!_ && !D && !A[0].isdir) {
			B = true;
		}
		if (Page.inViewMode(Page.VIEW_HOME)) {
			E = this._mToolbarMenuItems[Page.MENU_ANOTHER_DOWNLOAD];
		} else {
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				E = this._mToolbarMenuItems[Page.MENU_DOWNLOAD];
			}
		}
		if (E != null && typeof E != "undefined") {
			E.href = B ? Page.obtainDownloadURL(A[0]) : "#";
		}
	},
	resolveToolbarMenu : function (W) {
		if ($("#fileList .no-result-title").length > 0) {
			$(".all-files-headers").hide();
			Utilities.resolveLayout();
			return;
		} else {
			if ($(".all-files-headers:hidden").length) {
				$(".all-files-headers").show();
				Utilities.resolveLayout();
			}
		}
		var b = FileUtils.getModule() == "category",
		X = FileUtils.isListViewLocked(),
		G = X,
		c = FileUtils.getListViewCheckedItems(),
		f = c.length > 1,
		Y = c.length == 0;
		if (f) {
			$(FileUtils.getFileListDOM()).addClass("multi-sel");
		} else {
			$(FileUtils.getFileListDOM()).removeClass("multi-sel");
		}
		this.dispatchFileSelectionChange(f, b, G, Y, X, c);
		if (Page.inViewMode(Page.VIEW_HOME) || Page.VIEW_RECYCLE_BIN) {
			if (Y) {
				$(".files-header").hide();
				if (FileUtils._mViewMode == FileUtils.VIEW_GRID_ICON) {
					$("#fileThumbHeader").show();
				} else {
					if (FileUtils.inSearchMode()) {
						$("#sortColsHeader").show();
						$(".all-files-headers,#infiniteListView").addClass("searchMode");
					} else {
						$("#sortColsHeader").show();
						$(".all-files-headers,#infiniteListView").removeClass("searchMode");
					}
				}
			} else {
				if (Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {}
				else {
					var a = Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) ? "\u4e2a\u5916\u94fe" : "\u4e2a\u6587\u4ef6/\u6587\u4ef6\u5939";
					$(".files-header").hide();
					$("#fileActionHeader").show();
					$("#file_action_word").text("\u5df2\u9009\u4e2d" + c.length + a);
				}
			}
		}
		if (Page.inViewMode(Page.VIEW_HOME)) {
			var _ = null,
			O = false,
			J = false,
			I = false,
			V = false,
			B = 1,
			H = false,
			Q = false,
			P = false,
			d = false,
			Z = false,
			R = $("#editPicHint"),
			T = $("#barCmdEdit"),
			M;
			FileUtils.SELECT_PIC_NUM = 0;
			if (!Y) {
				for (var N = 0, A = c.length; N < A; N++) {
					_ = c[N];
					if (!I) {
						I = FileUtils.ifSpecialFolder(FileUtils.parseDirPath(_.path));
					}
					if (!V) {
						V = FileUtils.isInParticularDir(FileUtils.parseDirPath(_.path));
					}
					if (!Q) {
						Q = FileUtils.isTopParticularDir(FileUtils.parseDirPath(_.path));
					}
					if (!d) {
						d = _.isdir != 1 ? false : true;
					}
					if (!Z) {
						Z = FileUtils._checkIsBuyFile(_.path);
					}
					if (B) {
						B = FileUtils.isAllPic(_.category);
					}
				}
			}
			H = FileUtils.isRevisionCheck(c);
			if (B) {
				T.removeClass("none");
				if (B == 1) {
					T.find(".icon-edit-pic").removeClass("puzzle").addClass("prettify");
					T.find("b").text("\u7f8e\u5316");
				} else {
					T.find(".icon-edit-pic").removeClass("prettify").addClass("puzzle");
					T.find("b").text("\u62fc\u56fe");
				}
				disk.Context.requestFeature(disk.Context.FEATURE_STORAGE, {
					succ : function () {
						M = localStorage.getItem("edit_pic_hint");
						if (!M) {
							R.removeClass("none");
							var _,
							C = $("#file_action_buttons"),
							A = C[0].offsetLeft,
							B = C.width();
							_ = B / 2 + A + 50;
							R.css("left", _ + "px");
							localStorage.setItem("edit_pic_hint", "1");
						} else {
							R.addClass("none");
						}
					}
				});
			} else {
				R.addClass("none");
				setTimeout(function () {
					T.addClass("none");
				}, 10);
			}
			var D = !d && !Z;
			this.setToolbarMenuItemDisplay(Page.MENU_BAR_SELL_FILE, false);
			var L = !D && FileUtils.getListViewCheckedItems().length > 0;
			$("#barCmdSellFile")[(L ? "add" : "remove") + "Class"]("disabled");
			this.setToolbarMenuItemEnabled(Page.MENU_BAR_SELL_FILE, D);
			var F = !G && !f && !Y && !I && !O && !J && !Q;
			this.setToolbarMenuItemEnabled(Page.MENU_RENAME, F);
			var S = !G && !Y && !I && !Q;
			this.setToolbarMenuItemEnabled(Page.MENU_DELETE, S);
			if (FileUtils.inSearchMode()) {}
			else {
				$("#barCmdShareAll").removeClass("none");
				$("#barCmdDelete").removeClass("none");
			}
			var K = !G && !Y;
			this.setToolbarMenuItemEnabled(Page.MENU_SHARE, K);
			var U = !G && !Y;
			this.setToolbarMenuItemEnabled(Page.MENU_PUBLIC, U);
			var e = B ? true : false;
			this.setToolbarMenuItemEnabled(Page.MENU_PRINT, e);
			this.setToolbarMenuItemEnabled(Page.MENU_REVISION_LIST, H ? true : false);
			if (!this._mBarCmdTrigger) {
				this._mBarCmdTrigger = [document.getElementById("barDropdownTriggerShare"), document.getElementById("barDropdownTriggerMore")];
			}
			if (!K && !U) {
				$(this._mBarCmdTrigger[0]).addClass("disabled");
				$(this._mBarCmdTrigger[0]).parent().addClass("b-img-over-disabled");
			} else {
				$(this._mBarCmdTrigger[0]).removeClass("disabled");
				$(this._mBarCmdTrigger[0]).parent().removeClass("b-img-over-disabled");
			}
			var C = !G && !Y && !I;
			this.setToolbarMenuItemEnabled(Page.MENU_COPY, C);
			var E = !G && !Y && !I && !O && !J && !Q;
			this.setToolbarMenuItemEnabled(Page.MENU_MOVE, E);
			if (!F && !C && !E) {
				$(this._mBarCmdTrigger[1]).addClass("disabled");
			} else {
				$(this._mBarCmdTrigger[1]).removeClass("disabled");
			}
			this.setToolbarMenuItemEnabled(Page.MENU_ANOTHER_DOWNLOAD, !X && !Y);
			this.setToolbarMenuItemEnabled(Page.MENU_PLAY_ALL, !G && !Y);
			this.setToolbarMenuItemEnabled(Page.MENU_NEW_FOLDER, !G && W !== true && b !== true);
		} else {
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				this.setToolbarMenuItemEnabled(Page.MENU_DOWNLOAD, !G && !Y);
				this.setToolbarMenuItemEnabled(Page.MENU_TRANSFER, !G && !Y);
			} else {
				if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
					this.setToolbarMenuItemEnabled(Page.MENU_CANCEL_SHARE, !G && !Y);
				} else {
					if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
						this.setToolbarMenuItemEnabled(Page.MENU_RESTORE, !G && !Y);
						this.setToolbarMenuItemEnabled(Page.MENU_PERSIST_REMOVE, !G && !Y);
					}
				}
			}
		}
	},
	setToolbarMenuItemEnabled : function (_, A) {
		if (A) {
			$(this._mToolbarMenuItems[_]).removeClass("disabled");
		} else {
			$(this._mToolbarMenuItems[_]).addClass("disabled");
		}
	},
	isToolbarMenuItemEnabled : function (_) {
		var A = this._mToolbarMenuItems[_];
		return (typeof A != "undefined") && !$(A).hasClass("disabled");
	},
	setToolbarMenuItemDisplay : function (A, B) {
		var _;
		if (A === Page.MENU_COPY || A === Page.MENU_MOVE || A === Page.MENU_RENAME) {
			_ = $(this._mToolbarMenuItems[A]).parent();
		} else {
			_ = $(this._mToolbarMenuItems[A]);
		}
		if (B) {
			_.show();
		} else {
			_.hide();
		}
	},
	_resolveFileProperty : function (A, _) {},
	_getDomainUrl : function () {
		var _,
		A = window.location.host;
		if (A.indexOf("/") != -1) {
			urlArray = A.split("/");
			_ = urlArray[0];
		} else {
			_ = A;
		}
		_ = window.location.protocol + "//" + _;
		return _;
	},
	performSelectionDownload : function () {
		var B = FileUtils.getListViewCheckedItems(),
		F = disk.Context.getService(disk.Context.SERVICE_DOWNLOAD_PROXY);
		if (B.length === 0) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u60a8\u8fd8\u6ca1\u6709\u9009\u62e9\u4e0b\u8f7d\u7684\u6587\u4ef6",
				sticky : true,
				closeType : true
			});
			return true;
		}
		F.addUIVisibilityEventWatcher(function (_) {});
		if (B.length == 1 && !B[0].isdir) {
			var C = B[0];
			FileUtils.downloadCounter([C], true);
			F.download(C, 1);
			return false;
		} else {
			var E = [],
			G = false;
			if (false && FileUtils.isListViewFullyChecked() && Page.inViewMode(Page.VIEW_HOME)) {
				var D = FileUtils.getDirMgr().getDir();
				if (D.trim().length > 0 && D.indexOf("\u5206\u7c7b") === -1) {
					G = true;
					E.push({
						path : D
					});
				}
			}
			if (!G) {
				F.setPackName(FileUtils.parseDirFromPath(B[0].path), !B[0].isdir);
				for (var A = 0, _ = B.length; A < _; A++) {
					if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
						E.push({
							isdir : B[A].isdir,
							path : FileUtils.parseDirPath(B[A].path),
							size : B[A].size,
							fs_id : B[A].fs_id
						});
					} else {
						E.push({
							isdir : B[A].isdir,
							path : FileUtils.parseDirPath(B[A].path),
							size : B[A].size
						});
					}
				}
			}
			F.assignPureList(B);
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				F.download(E, true);
			} else {
				F.download(E);
			}
			FileUtils.downloadCounter(B, false);
			return false;
		}
	},
	performSelectPrint : function (_) {
		var A = function () {
			var C = disk.Context.getService(disk.Context.SERVICE_DIALOG);
			C.setVisible(true);
			var D = $(".alert-dialog .alert-dialog-commands .okay"),
			A = $(".alert-dialog .alert-dialog-commands .cancel"),
			B = D.html(),
			_ = A.html();
			D.html("<b>\u6211\u77e5\u9053\u4e86</b>");
			A.hide();
			C.setMessage("\u63d0\u793a", "<b>\u5c0a\u656c\u7684\u7528\u6237\uff0c\u6211\u4eec\u6682\u4e0d\u652f\u6301\u7b2c\u4e09\u65b9\u5e10\u53f7\uff08\u5982QQ\u3001\u65b0\u6d6a\u3001\u4eba\u4eba\uff09</b><br><b>\u7684\u7167\u7247\u51b2\u5370\u670d\u52a1\uff0c\u8bf7\u4f60\u8c05\u89e3</b><br/><b>\u6211\u4eec\u4f1a\u5c3d\u5feb\u63d0\u4f9b\u7b2c\u4e09\u65b9\u5e10\u53f7\u7684\u51b2\u5370\u670d\u52a1\uff0c\u8bf7\u4f60\u7ee7\u7eed\u5173\u6ce8\u767e\u5ea6\u4e91</b>", function () {
				this.setVisible(false);
				D.html(B);
				A.show();
			}, function () {
				this.setVisible(false);
				D.html(B);
				A.show();
			});
		},
		B = function () {
			if (FileUtils._mCloudPrint == null) {
				$.getScript("/res/static/js/buss/dialog/cloud_print_dialog.js?t=" + (new Date().getTime()), function (A) {
					if (A) {
						FileUtils._mCloudPrint = new disk.ui.CloudPrint();
						FileUtils._mCloudPrint.reBuild(_);
					} else {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u52a0\u8f7d\u5931\u8d25\uff0c\u8bf7\u5237\u65b0\u9875\u9762\u518d\u8bd5&hellip;",
							sticky : false
						});
					}
				});
			} else {
				FileUtils._mCloudPrint.reBuild(_);
			}
		};
		B();
	},
	_reqAddYunDeviceDialog : function (_) {
		var A = arguments.callee;
		FileUtils._mDiskLog.send({
			type : "add_yun_device_dialog"
		});
		if (!A.dialog) {
			ark.use({
				url : ["/util/jh.js", "/buss/dialog/add_yun_device_dialog.js", "/dialog/add-yun-device-dialog.css"],
				noDependency : false
			}, function () {
				if (A && !A.dialog) {
					A.dialog = new disk.ui.addYunDeviceDialog;
				}
				_(A.dialog);
			});
		} else {
			_(A.dialog);
		}
	},
	_reqYunDeviceDialog : function (_) {
		var A = arguments.callee;
		if (!A.dialog) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_LOADING,
				msg : "\u6b63\u5728\u52a0\u8f7d\u8bbe\u5907\u63a8\u9001\u5217\u8868\u7ec4\u4ef6&hellip;",
				sticky : false
			});
			ark.use({
				url : ["/util/jh.js", "/buss/dialog/yun_device_dialog.js", "/dialog/yun-device-dialog.css", "/page.css", "/buss/page.js"],
				noDependency : true
			}, function () {
				A.dialog = new disk.ui.yunDeviceDialog();
				_(A.dialog);
				Utilities.useCloseToast();
			});
		} else {
			_(A.dialog);
		}
	},
	performOpenDevice : function (_) {
		this._reqYunDeviceDialog(function (_) {
			_.show();
		});
	},
	performAddDevice : function (_) {
		this._reqAddYunDeviceDialog(function (_) {
			_.show();
		});
	},
	performRevisionList : function (_) {
		if (FileUtils._mRevisionList == null) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_LOADING,
				msg : "正在加载历史版本组件…",
				sticky : false
			});
			ark.use({
				url : ["/buss/dialog/revision_list_dialog.js", "/dialog/revision-dialog.css"],
				noDependency : false
			}, function () {
				Utilities.useCloseToast();
				if (typeof FileUtils._mRevisionList != "undefined") {
					FileUtils._mRevisionList = new disk.ui.RevisionList();
				} else {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u5386\u53f2\u7248\u672c\u7ec4\u4ef6\u52a0\u8f7d\u5931\u8d25\uff0c\u8bf7\u5237\u65b0\u9875\u9762\u518d\u8bd5&hellip;",
						sticky : false
					});
				}
				FileUtils._mRevisionList.reBuild(_);
			});
		} else {
			FileUtils._mRevisionList.reBuild(_);
		}
	},
	performAllDownload : function () {
		var _ = FileUtils.getListViewCheckedItems();
		if (_ && _.length > 0) {
			return this.performSelectionDownload();
		} else {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u5148\u9009\u62e9\u4e00\u4e0b\u4f60\u8981\u4e0b\u8f7d\u7684\u6587\u4ef6\u54e6",
				sticky : false
			});
		}
	},
	performAllTransfer : function () {
		var _ = FileUtils.getListViewCheckedItems();
		if (_ && _.length > 0 && _.length <= 1000) {
			FileUtils.transferFiles(_);
		} else {
			if (_.length > 1000) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc71000\u4e2a",
					sticky : false
				});
			} else {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u5148\u9009\u62e9\u4e00\u4e0b\u4f60\u8981\u4fdd\u5b58\u7684\u6587\u4ef6\u54e6",
					sticky : false
				});
			}
		}
	},
	performSelectionTransfer : function () {
		var _ = FileUtils.getListViewCheckedItems();
		if (_.length > 1000) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc71000\u4e2a",
				sticky : false
			});
		} else {
			FileUtils.transferFiles(_);
		}
	},
	performSelectionShowFolder : function () {
		if (FileUtils.getModule() === "category" && FileUtils.getType() === 2) {
			FileUtils.removeTimelineView();
		}
		var _ = FileUtils.getListViewCheckedItems();
		if (_.length == 1) {
			var B = FileUtils.parseFullDirFromPath(_[0].path),
			A = FileUtils._mInfiniteListView;
			if (A) {
				A.setItemsChecked(false);
			}
			A = FileUtils._mInfiniteGridView;
			if (A) {
				A.setItemsChecked(false);
			}
			FileUtils.historyNavigate(B == disk.util.DirectoryLocation.DIR_ROOT ? {}

				 : {
				path : B
			});
		}
	},
	performSelectionDelete : function () {
		var _ = FileUtils.getListViewCheckedItems();
		if (_.length > 100) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc7100\u4e2a",
				sticky : false
			});
		} else {
			FileUtils.deleteFiles(_);
		}
	},
	performSelectionEditPic : function (A) {
		var F = disk.ui.Utilities.getFlashVersion();
		if (F == null || parseFloat(F, 10) < 10.2) {
			var G = $(".alert-dialog .alert-dialog-commands .okay"),
			L = $(".alert-dialog .alert-dialog-commands .cancel"),
			M = G.html(),
			_ = L.html(),
			K = disk.Context.getService(disk.Context.SERVICE_DIALOG);
			K.setVisible(true);
			G.html("<b>\u5b89\u88c5</b>");
			K.setMessage("\u53cb\u597d\u63d0\u793a", "\u8981\u4f7f\u7528\u56fe\u7247\u5728\u7ebf\u7f16\u8f91\u529f\u80fd\uff0c\u60a8\u9700\u8981\u5b89\u88c5Flash\u64ad\u653e\u566810.2.0 \u6216\u66f4\u9ad8\u7684\u7248\u672c\u3002", function () {
				window.open("http://get.adobe.com/cn/flashplayer");
				this.setVisible(false);
				G.html(M);
			}, function () {
				this.setVisible(false);
				G.html(M);
			});
		} else {
			var D = FileUtils.getListViewCheckedItems(),
			B = [],
			N = "",
			H = 12;
			for (var I = 0, J = D.length; I < J; I++) {
				var E = D[I];
				if (E.category == 3) {
					if (J == 1) {
						N = encodeURIComponent(E.path);
						B.push(encodeURIComponent(E.dlink));
						if (typeof _hmt == "object") {
							if (A == "right") {
								_hmt.push(["_trackEvent", "\u56fe\u7247\u5728\u7ebf\u7f16\u8f91", "\u5217\u8868\u70b9\u51fb\u7f8e\u5316\u5165\u53e3\u7684\u6b21\u6570"]);
							} else {
								_hmt.push(["_trackEvent", "\u56fe\u7247\u5728\u7ebf\u7f16\u8f91", "\u64cd\u4f5c\u680f\u70b9\u51fb\u7f8e\u5316\u5165\u53e3\u7684\u6b21\u6570"]);
							}
						}
					} else {
						if (B.length <= H) {
							var C = disk.setParam("size", "c1000_u800", E.thumbs["url1"]);
							B.push(encodeURIComponent(C));
							if (typeof _hmt == "object") {
								if (A == "right") {
									_hmt.push(["_trackEvent", "\u56fe\u7247\u5728\u7ebf\u7f16\u8f91", "\u5217\u8868\u70b9\u51fb\u62fc\u56fe\u5165\u53e3\u7684\u6b21\u6570"]);
								} else {
									_hmt.push(["_trackEvent", "\u56fe\u7247\u5728\u7ebf\u7f16\u8f91", "\u64cd\u4f5c\u680f\u70b9\u51fb\u62fc\u56fe\u5165\u53e3\u7684\u6b21\u6570"]);
								}
							}
						}
					}
				}
			}
			disk.Context.requestFeature(disk.Context.FEATURE_STORAGE, {
				succ : function () {
					localStorage.setItem("picArray", B.join(","));
					localStorage.setItem("picPath", N);
					window.open("/disk/editpic");
				}
			});
		}
	},
	performSelectionCancelShare : function () {
		var _ = FileUtils.getListViewCheckedItems();
		if (_.length > 100) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc7100\u4e2a",
				sticky : false
			});
		} else {
			FileUtils.cancelShare(_, "share");
		}
	},
	performSelectionMoveCopy : function (D, C) {
		var E = this;
		if (typeof C == "undefined") {
			C = FileUtils.getListViewCheckedItems();
		}
		if (C.length > 100) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc7100\u4e2a",
				sticky : false
			});
		} else {
			if (D == disk.ui.MoveCopyDialog.COPY) {
				var B = 0;
				for (var A = 0, _ = C.length; A < _; A++) {
					B += C[A].size ? B : 0;
					if (!Utilities.hasEnoughSpacing(B)) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u590d\u5236\u5931\u8d25\uff0c\u5269\u4f59\u7a7a\u95f4\u4e0d\u8db3",
							sticky : false
						});
						return;
					}
				}
			}
			if (!E._mMoveCopyDialog) {
				E._mMoveCopyDialog = new disk.ui.MoveCopyDialog();
			}
			E._mMoveCopyDialog.onRefuse = function (_) {
				FileUtils.switchToDir(FileUtils.getDirMgr().getDir());
			};
			E._mMoveCopyDialog.onClose = function () {};
			E._mMoveCopyDialog.onConsent = function (D, F) {
				if (disk.DEBUG) {
					console.log("before move selected files and dirs to ", "[", F, "]", "and action type ", D);
				}
				var B = this.getCacheItems();
				if (B.length > 0) {
					if (F == FileUtils.getDirMgr().getDir()) {
						if (disk.DEBUG) {
							console.log(">>>>we are hitting the same dir, reject this request");
						}
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u60a8\u8981" + (D == disk.ui.MoveCopyDialog.MOVE ? "\u79fb\u52a8" : "\u590d\u5236") + "\u7684\u6587\u4ef6\u5df2\u7ecf\u5b58\u5728\u4e8e\u76ee\u6807\u8def\u5f84",
							sticky : false
						});
						FileUtils.switchToDir(F);
						return;
					}
					for (var A = 0, C, _ = B.length; A < _; A++) {
						C = FileUtils.parseDirPath(B[A].path);
						if (disk.util.DirectoryLocation.isDecendentsOf(F, C)) {
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : "\u4e0d\u80fd\u5c06\u6587\u4ef6" + (D == disk.ui.MoveCopyDialog.MOVE ? "\u79fb\u52a8" : "\u590d\u5236") + "\u5230\u81ea\u8eab\u6216\u5176\u5b50\u76ee\u5f55\u4e0b",
								sticky : false
							});
							FileUtils.switchToDir(FileUtils.getDirMgr().getDir());
							return;
						}
					}
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_LOADING,
						msg : "\u6b63\u5728" + (D == disk.ui.MoveCopyDialog.MOVE ? "\u79fb\u52a8" : "\u590d\u5236") + "\u6587\u4ef6\uff0c\u8bf7\u7a0d\u5019&hellip;",
						sticky : true
					});
					FileUtils.sendMoveCopyFileMessage(D == disk.ui.MoveCopyDialog.MOVE, B, F == disk.util.DirectoryLocation.DIR_ROOT ? "/" : F, function (K) {
						try {
							var A = disk.Context.getService(disk.Context.SERVICE_TOAST),
							B = (D == disk.ui.MoveCopyDialog.MOVE ? "\u79fb\u52a8" : "\u590d\u5236") + "\u6587\u4ef6\u5931\u8d25",
							J = null;
							A.setVisible(false);
							var L = $.parseJSON(K);
							if (L.errno == 0) {
								B = (D == disk.ui.MoveCopyDialog.MOVE ? "\u79fb\u52a8" : "\u590d\u5236") + "\u6587\u4ef6\u6210\u529f";
							} else {
								if (L.errno == 12) {
									J = E.dealCopyMoveBatchCallback1(L, D);
									if (J) {}
									else {
										B = "\u90e8\u5206\u6587\u4ef6" + (D == disk.ui.MoveCopyDialog.MOVE ? "\u79fb\u52a8" : "\u590d\u5236") + "\u6210\u529f";
									}
								}
							}
							if (disk.DEBUG) {
								console.log("move or copy files success, prepare to update local cache");
							}
							if (FileUtils.getModule() == "category" || FileUtils.inSearchMode()) {
								FileUtils.getLocalCache().removeAll();
								FileUtils.getLocalCache().removeCategorys();
								FileUtils.clearAllDocData();
								A.pending(function () {
									Utilities.useToast(J ? J : {
										toastMode : disk.ui.Toast.MODE_SUCCESS,
										msg : B,
										sticky : false
									});
								});
								if (FileUtils.getModule() == "category") {
									FileUtils.triggerType(FileUtils.getType());
								} else {
									if (FileUtils.inSearchMode()) {
										FileUtils.clearAllDocData();
										FileUtils.issueProxyLoading(true, false);
										var G = FileUtils.resolveCurrentDirKey();
										FileUtils.loadDir(G == FileUtils.ROOT_ID ? "" : G);
									} else {
										FileUtils.clearAllDocData();
										FileUtils.issueProxyLoading(true, false);
										FileUtils.loadDir(FileUtils.resolveCurrentDirKey());
									}
								}
								Utilities.scheduleQueryQuota();
								Utilities.useToast({
									toastMode : disk.ui.Toast.MODE_LOADING,
									msg : "\u6b63\u5728\u52a0\u8f7d\u6570\u636e\uff0c\u8bf7\u7a0d\u5019&hellip;",
									sticky : true
								});
							} else {
								var C = disk.util.FileCache.KEY_MODE_NORMAL,
								N = FileUtils.resolveCurrentDirKey(),
								H = [];
								if (D == disk.ui.MoveCopyDialog.MOVE) {
									infos = L.info;
									for (var _ = infos.length, I = _ - 1; I >= 0; I--) {
										if (infos[I].errno === 0) {
											H.push(infos[I]);
										}
									}
								} else {}

								var F = FileUtils.getLocalCache().movecopyByMode(C, N, D, H);
								if (F) {
									FileUtils.clearAllDocData();
									var M = FileUtils.getListView();
									M.setItemsChecked(false);
									K = FileUtils.getLocalCache().obtainByMode(C, N);
									if (FileUtils.getViewMode() != FileUtils.VIEW_LIST) {
										K = FileUtils.setGroupData(K, FileUtils._mGridStep);
									}
									M.updateBackedData(K);
									Page.obtain().resolveToolbarMenu();
									Utilities.scheduleQueryQuota();
								}
								Utilities.useToast(J ? J : {
									toastMode : disk.ui.Toast.MODE_SUCCESS,
									msg : B,
									sticky : false
								});
							}
						} catch (O) {
							if (disk.DEBUG) {
								console.log("error on move or copy files ", O.message);
							}
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : (D == disk.ui.MoveCopyDialog.MOVE ? "\u79fb\u52a8" : "\u590d\u5236") + "\u6587\u4ef6\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
								sticky : false
							});
						}
						this._mPendingHighlights = null;
					});
				}
			};
			if (disk.DEBUG) {
				console.log("before action type>>>", D);
			}
			E._mMoveCopyDialog.setVisible(true);
			E._mMoveCopyDialog.setActionType(D);
			E._mMoveCopyDialog.setCacheItems(C);
		}
	},
	dealCopyMoveBatchCallback : function (E, H) {
		var G = false,
		D = false,
		I = false,
		C = false,
		F = null,
		B = FileUtils.getListViewCheckedItems();
		for (var A = 0, _ = E.info.length; A < _; A++) {
			F = E.info[A];
			if (F.errno == -10) {
				G = true; //-10: 失败，剩余空间不足
			} else {
				if (F.errno == -8) {
					D = true; //-8: 失败，文件已存在于目标文件夹中
				} else {
					if (F.errno == 0) {
						I = true;
					} else {
						if (F.errno == -9) {
							C = true; //-9: 失败，文件不存在
						}
					}
				}
			}
		}
		if (!G && !D && !C) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败，请稍候重试",
				sticky : true,
				closeType : true
			});
			return false;
		}
		if (E.info.length == B.length && !I) {
			if (G) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败，剩余空间不足",
					sticky : true,
					closeType : true
				});
			} else {
				if (D) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : (H == disk.ui.MoveCopyDialog.MOVE ? "\移动" : "复制") + "失败，文件已存在于目标文件夹中",
						sticky : false
					});
				} else {
					if (C) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败，文件不存在",
							sticky : true,
							closeType : true
						});
					}
				}
			}
			return false;
		} else {
			if (G) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "文件" + (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败, 使用空间已满",
					sticky : false
				});
			} else {
				if (D) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "文件" + (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败, 部分文件已存在",
						sticky : false
					});
					FileUtils.clearCurrentDirCache();
				} else {
					if (C) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "文件" + (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败, 部分文件不存在",
							sticky : false
						});
						FileUtils.clearCurrentDirCache();
					}
				}
			}
			return false;
		}
		return true;
	},
	dealCopyMoveBatchCallback1 : function (E, H) {
		var G = false,
		D = false,
		I = false,
		C = false,
		F = null,
		B = FileUtils.getListViewCheckedItems();
		for (var A = 0, _ = E.info.length; A < _; A++) {
			F = E.info[A];
			if (F.errno == -10) {
				G = true;
			} else {
				if (F.errno == -8) {
					D = true;
				} else {
					if (F.errno == 0) {
						I = true;
					} else {
						if (F.errno == -9) {
							C = true;
						}
					}
				}
			}
		}
		if (!G && !D && !C) {
			return {
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败，请稍候重试",
				sticky : true,
				closeType : true
			};
		}
		if (E.info.length == B.length && !I) {
			if (G) {
				return {
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败，剩余空间不足",
					sticky : true,
					closeType : true
				};
			} else {
				if (D) {
					return {
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败，文件已存在于目标文件夹中",
						sticky : false
					};
				} else {
					if (C) {
						return {
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败，文件不存在",
							sticky : true,
							closeType : true
						};
					} else {
						return {
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败",
							sticky : true,
							closeType : true
						};
					}
				}
			}
		} else {
			if (G) {
				return {
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u6587\u4ef6" + (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败, 使用空间已满",
					sticky : false
				};
			} else {
				if (D) {
					return {
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "部分文件已存在，" + (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败 ",
						sticky : false
					};
				} else {
					if (C) {
						return {
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "部分文件不存在, " + (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败",
							sticky : false
						};
					} else {
						return {
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "文件" + (H == disk.ui.MoveCopyDialog.MOVE ? "移动" : "复制") + "失败",
							sticky : false
						};
					}
				}
			}
		}
		return null;
	},
	performSelectionShareAll : function (E) {
		var D = 0;
		$.getJSON(disk.api.RestAPI.LINK_COUNT + "&type=1&t=" + (new Date().getTime()), function (A) {
			var B = parseInt(A.count);
			if (B >= 100000) {
				var _ = disk.Context.getService(disk.Context.SERVICE_DIALOG);
				_.setVisible(true);
				$(".cancel", _._mUI.pane).hide();
				_.setMessage("\u5206\u4eab\u9519\u8bef", "\u4f60\u5df2\u7ecf\u5206\u4eab\u4e86100000\u6761\uff0c\u4e0d\u80fd\u518d\u6b21\u5206\u4eab\uff01", function () {
					$(".cancel", _._mUI.pane).show();
					this.setVisible(false);
				}, function () {
					$(".cancel", _._mUI.pane).show();
				});
			}
		});
		if (D == 0) {
			if (E.length > 100) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc7100\u4e2a",
					sticky : false
				});
			} else {
				var G = this,
				J = [],
				K = null,
				A = false,
				I = false;
				for (var H = 0, _ = E.length; H < _; H++) {
					K = E[H];
					J.push(K.server_filename);
				}
				var F = function () {
					if (A && I) {
						G._mShareDialog.setFileList(E);
					}
				},
				C = ("flag_fillusername_shareall_" + FileUtils.sysUID).replace(/@|\*/g, "");
				if (!window.localStorage.getItem(C)) {
					window.localStorage.setItem(C, 1);
					ark.use({
						url : ["/ui/fill_username_dialog"],
						noDependency : true
					}, function () {
						G._mfillUserNameDialog = disk.Context.getService(disk.Context.SERVICE_FILLNAME_DIALOG);
						G._mfillUserNameDialog.checkHasUserName();
						G._mfillUserNameDialog.onHasUserName = function () {
							A = true;
							F();
						};
						G._mfillUserNameDialog.onNotHasUserName = function () {
							this.setVisible(true);
						};
						G._mfillUserNameDialog.onFillUserNameSuccess = function () {
							disk.ui.header.setNickname(this.username);
							this.setVisible(false);
							A = true;
							F();
						};
						G._mfillUserNameDialog.onHide = function () {
							A = true;
							F();
						};
					});
				} else {
					A = true;
					F();
				}
				var B = function () {
					I = true;
					if (G._mShareDialog == null) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_LOADING,
							msg : "\u6b63\u5728\u52a0\u8f7d\u5206\u4eab\u529f\u80fd,\u8bf7\u7a0d\u5019\u2026\u2026",
							sticky : false
						});
						ark.use({
							url : ["/res/static/js/buss/sharefriend/contentController.js", "/res/static/js/buss/sharefriend/share_dialog.js", "/res/static/css/sharefriend/shareDialog.css"],
							noDependency : false,
							enforceUrl : true
						}, function () {
							Utilities.useCloseToast();
							if (typeof disk.ui.ShareDialog != "undefined") {
								G._mShareDialog = new disk.ui.ShareDialog();
								F();
							} else {
								Utilities.useToast({
									toastMode : disk.ui.Toast.MODE_CAUTION,
									msg : "\u5206\u4eab\u529f\u80fd\u52a0\u8f7d\u5931\u8d25,\u8bf7\u7a0d\u5019\u91cd\u8bd5",
									sticky : false
								});
							}
						});
					} else {
						F();
					}
				};
				$.get(disk.api.RestAPI.normalize(disk.api.RestAPI.INBOX_OBJECT_CHECKANTISPAM, FileUtils.bdstoken), {
					path_array : $.stringify(J)
				}, function (C) {
					if (C == "" || C.errno == 0 || typeof C.errno === "undefined") {
						B();
					} else {
						var A = "\u53d1\u9001\u7684\u6587\u4ef6\u540d\u4e2d\u6709\u654f\u611f\u8bcd\uff0c\u4f18\u5316\u4e00\u4e0b\u5427";
						if (C.errno == -52) {
							var _ = C.antispam[0];
							_ = _.length <= 6 ? _ : _.substring(0, 6) + "...";
							A = "\u201c" + (_ || "\u53d1\u9001") + "\u201d\u7684\u6587\u4ef6\u540d\u4e2d\u6709\u654f\u611f\u8bcd\uff0c\u4f18\u5316\u4e00\u4e0b\u5427";
						} else {
							if (C.errno == -6) {
								A = "\u8bf7\u91cd\u65b0\u767b\u5f55";
							} else {
								if (C.errno == -72) {
									A = "\u53d1\u9001\u7684\u6587\u4ef6\u4e0d\u5b58\u5728\uff0c\u68c0\u67e5\u4e00\u4e0b\u5427";
								}
							}
						}
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : A,
							sticky : false
						});
					}
				}).error(function () {
					B();
				});
			}
		}
	},
	performUpload : function (_) {
		var B = this,
		A = disk.util.HistoryManager.interpretCurrentWindow(true);
		if (Object.prototype.toString.call(A) == "[object Array]" && A[1] != "undefined" && A[1].type == "2") {
			FileUtils._mDiskLog.send({
				type : "click_upload_button_on_timeline"
			});
		}
		if (this._mUploadDialog == null) {
			return;
		}
		if (_) {
			return B._mUploadDialog;
		}
		if (B._mUploadDialog.getFileUploader()instanceof disk.ui.OpenSourceFlashFileUploader) {
			B._mUploadDialog.setVisible(true);
		}
		$("#barCmdUpload").parent().removeClass("b-img-over");
		B._mUploadDialog.getFileUploader().dispatchChoosingFiles();
	},
	performGlobalSearch : function (A, _) {
		FileUtils.createCategoryUI(FileUtils.getModule(), FileUtils.MODULE_DIR);
		var B = FileUtils._mDirMgr.getDir();
		if (A == "") {
			if (_ === true) {
				if (disk.DEBUG) {
					console.log("Receive General Message");
				}
				FileUtils.getDirMgr().cd(disk.util.DirectoryLocation.DIR_ROOT, false);
			} else {
				FileUtils.historyNavigate({});
			}
			return;
		}
		Utilities.useToast({
			toastMode : disk.ui.Toast.MODE_LOADING,
			msg : "获取搜索结果&hellip;", /*&hellip;解码后是："…"，即3个点的中文省略号*/
			sticky : true
		});
		FileUtils.sendSearchMessage(B, A, function (_) {
			FileUtils.switchLayoutMode(FileUtils.MODE_SEARCH);
			var C = null;
			try {
				C = $.parseJSON(_);
			} catch (D) {}

			if (C.errno == 2) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "网络错误，请稍候重试",
					sticky : true,
					closeType : true
				});
			} else {
				if (C == null || C.errno != 0 || !C.list) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "网络错误，请稍候重试",
						sticky : true,
						closeType : true
					});
				} else {
					disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
					FileUtils._mDirMgr._mAfterSearch = false;
					FileUtils._mDirMgr.cd("", false, true);
					var B = '\u641c\u7d22:"' + A + '"';
					FileUtils.getLocalCache().add(B, C.list);
					FileUtils._mDirMgr.cd(B, true);
					FileUtils._mDirMgr._mAfterSearch = true;
					if (C.list.length == 0) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "没有找到匹配的结果",
							sticky : false
						});
					}
					if (FileUtils._mLastSearchKey != null && B != FileUtils._mLastSearchKey) {
						FileUtils.getLocalCache().muteRemove(FileUtils._mLastSearchKey);
					}
					FileUtils._mLastSearchKey = B;
				}
			}
		});
	},
	performCategory : function (_) {
		if (!_) {
			$("#tab-home").addClass("on");
			$("#aside-menu a.b-no-ln").removeClass("on");
			disk.util.HistoryManager.getDefault().navigate(FileUtils.MODULE_DIR, null);
		} else {
			if (FileUtils._mInfiniteGridView && FileUtils._mInfiniteGridView.isAllItemChecked()) {
				FileUtils._mInfiniteGridView.setItemsChecked(false);
			}
			if (FileUtils._mInfiniteListView && FileUtils._mInfiniteListView.isAllItemChecked()) {
				FileUtils._mInfiniteListView.setItemsChecked(false);
			}
			if (_ == 2) {
				FileUtils.switchViewMode(FileUtils.VIEW_GRID_ICON, _);
			} else {
				if (_ == 3) {
					FileUtils.switchViewMode(FileUtils.VIEW_GRID_ICON, _);
				} else {
					if ((FileUtils.getViewMode() != FileUtils.USER_VIEW_MODE)) {
						FileUtils.switchViewMode(FileUtils.USER_VIEW_MODE);
					}
				}
			}
			FileUtils.bindCategoryClickLog(_);
			FileUtils.triggerType(_ - 0);
		}
	},
	performType : function (_) {
		disk.util.HistoryManager.getDefault().navigate(FileUtils.MODULE_CATEGORY, {
			type : FileUtils.TYPES[_][0]
		});
	},
	performSelectionRestore : function () {
		if (disk.DEBUG) {
			console.log("Selection Restore");
		}
		var A = FileUtils.getListViewCheckedItems(),
		B = FileUtils;
		if (A.length == 0) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u8fd8\u539f\u6587\u4ef6\u6ca1\u6709\u9009\u62e9",
				sticky : true,
				closeType : true
			});
			return false;
		} else {
			if (A.length > 100) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc7100\u4e2a",
					sticky : false
				});
				return false;
			}
		}
		var _ = disk.Context.getService(disk.Context.SERVICE_DIALOG);
		_.setVisible(true);
		_.setMessage("\u786e\u8ba4\u8fd8\u539f", "\u786e\u8ba4\u8fd8\u539f\u9009\u4e2d\u7684\u6587\u4ef6\uff1f", function () {
			var C = {};
			this.setVisible(false);
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_LOADING,
				msg : "\u6b63\u5728\u8fd8\u539f\u6570\u636e&hellip;",
				sticky : true
			});
			C.fidlist = [];
			for (var B = 0, _ = A.length; B < _; B++) {
				C.fidlist.push(A[B].fs_id);
			}
			C.fidlist = "[" + C.fidlist + "]";
			$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.RESTORE_RECYCLE + "&t=" + (new Date().getTime()), FileUtils.bdstoken), C, function (H) {
				var D = null,
				_ = disk.ui.Toast.MODE_SUCCESS;
				if (H.errno == 0) {
					D = "\u8fd8\u539f\u6587\u4ef6\u6210\u529f";
				} else {
					if (H.errno == 12) {
						var I = H.faillist,
						B = 0,
						C = I.length,
						F,
						G;
						for (; B < C; B++) {
							if (I[B].errno == 0) {
								continue;
							}
							switch (I[B].errno) {
							case  - 8:
								F = "\u76ee\u5f55\u5b58\u5728\u540c\u540d\u6587\u4ef6<br/>";
								break;
							case  - 9:
								F = "\u6587\u4ef6\u88ab\u6240\u6709\u8005\u5220\u9664\uff0c\u5237\u65b0\u540e\u518d\u8bd5";
								break;
							case 5:
								F = "\u76ee\u5f55\u5b58\u5728\u540c\u540d\u6587\u4ef6";
								break;
							case  - 10:
								F = "\u767e\u5ea6\u4e91\u7a7a\u95f4\u5df2\u6ee1";
								break;
							default:
								F = "\u670d\u52a1\u5668\u9519\u8bef";
								break;
							}
							break;
						}
						D = "\u8fd8\u539f\u6587\u4ef6\u5931\u8d25\uff0c" + F;
						_ = disk.ui.Toast.MODE_CAUTION;
					} else {
						var E = disk.util.ErrorMessage[H.errno] || "\u672a\u77e5\u9519\u8bef\uff0cerrno:" + H.errno;
						if (H.errno == -9) {
							E = "\u6587\u4ef6\u6216\u76ee\u5f55\u4e0d\u5b58\u5728";
						}
						D = E;
						_ = disk.ui.Toast.MODE_CAUTION;
					}
				}
				var A = disk.Context.getService(disk.Context.SERVICE_TOAST);
				A.setVisible(false);
				A.pending(function () {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_SUCCESS,
						msg : D,
						sticky : false
					});
				});
				FileUtils.getLocalCache().removeAll();
				FileUtils.issueProxyLoading(true);
				FileUtils.loadDir(FileUtils.ROOT_ID);
				Utilities.scheduleQueryQuota();
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u52a0\u8f7d\u6570\u636e\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
			}, "json").error(function () {
				var _ = disk.Context.getService(disk.Context.SERVICE_TOAST);
				_.setVisible(false);
				_.pending(function () {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u7f51\u7edc\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
						sticky : false
					});
				});
				FileUtils.getLocalCache().removeAll();
				FileUtils.issueProxyLoading(true);
				FileUtils.loadDir(FileUtils.ROOT_ID);
				Utilities.scheduleQueryQuota();
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u52a0\u8f7d\u6570\u636e\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
			});
		});
	},
	performSelectionRemoval : function (A) {
		if (disk.DEBUG) {
			console.log("Selection Removal " + A);
		}
		if (typeof A !== "undefined") {
			var _ = disk.Context.getService(disk.Context.SERVICE_DIALOG);
			_.setVisible(true);
			_.setMessage("\u6e05\u7a7a\u56de\u6536\u7ad9", "\u786e\u8ba4\u6e05\u7a7a\u56de\u6536\u7ad9\uff1f", function () {
				this.setVisible(false);
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u5220\u9664\u6570\u636e&hellip;",
					sticky : true
				});
				$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.CLEAR_RECYCLE + "&t=" + (new Date().getTime()), FileUtils.bdstoken), function (A) {
					if (A.errno == 0) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_SUCCESS,
							msg : "\u56de\u6536\u7ad9\u5df2\u6e05\u7a7a",
							sticky : false
						});
						FileUtils.clearCurrentDirCache();
						FileUtils.load(FileUtils.resolveCurrentDirKey());
						Utilities.scheduleQueryQuota();
					} else {
						var _ = disk.util.ErrorMessage[A.errno] || "\u672a\u77e5\u9519\u8bef\uff0cerrno:" + A.errno;
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : _,
							sticky : false
						});
					}
				}, "json").error(function () {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u7f51\u7edc\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u518d\u8bd5",
						sticky : false
					});
				});
			});
			return;
		}
		var C = FileUtils,
		B = FileUtils.getListViewCheckedItems();
		if (B.length == 0) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u5220\u9664\u6587\u4ef6\u6ca1\u6709\u9009\u62e9",
				sticky : true,
				closeType : true
			});
			return false;
		} else {
			if (B.length > 100) {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_CAUTION,
					msg : "\u4e00\u6b21\u64cd\u4f5c\u6587\u4ef6\u4e0d\u53ef\u8d85\u8fc7100\u4e2a",
					sticky : false
				});
				return false;
			}
		}
		_ = disk.Context.getService(disk.Context.SERVICE_DIALOG);
		_.setVisible(true);
		_.setMessage("\u5f7b\u5e95\u5220\u9664", "\u6587\u4ef6\u5220\u9664\u540e\u5c06\u65e0\u6cd5\u6062\u590d\uff0c\u60a8\u786e\u8ba4\u8981\u5f7b\u5e95\u5220\u9664\u6240\u9009\u6587\u4ef6\u5417\uff1f", function () {
			this.setVisible(false);
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_LOADING,
				msg : "\u6b63\u5728\u5220\u9664\u6570\u636e&hellip;",
				sticky : true
			});
			var C = {};
			C.fidlist = [];
			for (var A = 0, _ = B.length; A < _; A++) {
				C.fidlist.push(B[A].fs_id);
			}
			C.fidlist = "[" + C.fidlist + "]";
			$.post(disk.api.RestAPI.normalize(disk.api.RestAPI.DELETE_RECYCLE, FileUtils.bdstoken), C, function (C) {
				var B = null,
				_ = disk.ui.Toast.MODE_SUCCESS;
				if (C.errno == 0) {
					B = "彻底删除文件成功";
				} else {
					B = disk.util.ErrorMessage[C.errno] || "未知错误，errno:" + C.errno;
					_ = disk.ui.Toast.MODE_CAUTION;
				}
				var A = disk.Context.getService(disk.Context.SERVICE_TOAST);
				A.setVisible(false);
				A.pending(function () {
					Utilities.useToast({
						toastMode : _,
						msg : B,
						sticky : false
					});
				});
				FileUtils.getLocalCache().removeAll();
				FileUtils.issueProxyLoading(true);
				FileUtils.loadDir(FileUtils.ROOT_ID);
				Utilities.scheduleQueryQuota();
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u52a0\u8f7d\u6570\u636e\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
			}, "json").error(function () {
				var _ = disk.Context.getService(disk.Context.SERVICE_TOAST);
				_.setVisible(false);
				_.pending(function () {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u7f51\u7edc\u9519\u8bef\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
						sticky : false
					});
				});
				FileUtils.getLocalCache().removeAll();
				FileUtils.issueProxyLoading(true);
				FileUtils.loadDir(FileUtils.ROOT_ID);
				Utilities.scheduleQueryQuota();
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u52a0\u8f7d\u6570\u636e\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
			});
		});
	},
	dispatchActionPerformed : function (B, E, C) {
		if (Page.inViewMode(Page.VIEW_HOME)) {
			switch (B) {
			case Page.MENU_RENAME:
				switch (FileUtils._mViewMode) {
				case FileUtils.VIEW_GRID_ICON:
				case FileUtils.VIEW_GRID_THUMB:
					var A = FileUtils._mInfiniteGridView.getCheckedItems();
					if (A.length > 0) {
						FileUtils.prepareFolderBuilder(FileUtils.UPDATE, A[0], FileUtils._mInfiniteGridView.getFirstCheckedChild(), false);
					}
					break;
				case FileUtils.VIEW_LIST:
				default:
					A = FileUtils._mInfiniteListView.getCheckedItems();
					if (A.length > 0) {
						FileUtils.prepareFolderBuilder(FileUtils.UPDATE, A[0], FileUtils._mInfiniteListView.getFirstCheckedChild(), false);
					}
					break;
				}
				break;
			case Page.MENU_DELETE:
				this.performSelectionDelete();
				break;
			case Page.MENU_SHARE:
				var H = this,
				D = FileUtils.getListViewCheckedItems();
				H.performSelectionShareAll(D);
				break;
			case Page.MENU_DEVICE:
			case Page.MENU_PUBLIC:
				var D = FileUtils.getListViewCheckedItems(),
				H = this,
				G = C;
				if (G.preventDefault) {
					G.preventDefault();
				}
				if (G.stopPropagation) {
					G.stopPropagation();
				}
				return true;
			case Page.MENU_COPY:
			case Page.MENU_MOVE:
				var _ = E.getAttribute("_data") || C.target.getAttribute("_data"),
				F,
				H = this;
				if (Page.obtain()._mMoveCopyDialog == null) {
					ark.use({
						url : ["/buss/dialog/move_copy_dialog.js", "/dialog/copy-move-dialog.css"],
						noDependency : true
					}, function () {
						Utilities.useCloseToast();
						if (typeof disk.ui.MoveCopyDialog != "undefined") {
							Page.obtain()._mMoveCopyDialog = new disk.ui.MoveCopyDialog();
							F = (_.indexOf("copy") || _.indexOf("copy")) != -1 ? disk.ui.MoveCopyDialog.COPY : disk.ui.MoveCopyDialog.MOVE;
							H.performSelectionMoveCopy(F);
						} else {
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : "\u590d\u5236\u79fb\u52a8\u7ec4\u4ef6\u52a0\u8f7d\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
								sticky : false
							});
						}
					});
				} else {
					F = (_.indexOf("copy") || _.indexOf("copy")) != -1 ? disk.ui.MoveCopyDialog.COPY : disk.ui.MoveCopyDialog.MOVE;
					H.performSelectionMoveCopy(F);
				}
				break;
			case Page.MENU_UPLOAD:
				this.performUpload();
				break;
			case Page.MENU_NEW_FOLDER:
				FileUtils.prepareFolderBuilder(FileUtils.CREATE);
				break;
			case Page.MENU_SELL_FILE_BIG:
				Page.obtain().installSellDialog();
				break;
			case Page.MENU_DEVICE:
				break;
			case Page.MENU_SELL_FILE:
				Page.obtain().installSellDialog();
				break;
			case Page.MENU_BAR_SELL_FILE:
				Page.obtain().installSellDialog();
				break;
			case Page.MENU_ANOTHER_DOWNLOAD:
				return this.performSelectionDownload();
			case Page.MENU_NEW_OFFLINE:
				if (FileUtils._mNewOffline == null) {
					FileUtils._mNewOffline = disk.ui.NewOfflineDialog.obtain();
				}
				FileUtils._mNewOffline.setVisible(true);
				break;
			case Page.MENU_OFFLINE_LIST:
				if (FileUtils._mOfflineList == null) {
					FileUtils._mOfflineList = disk.ui.OfflineListDialog.obtain();
				}
				FileUtils._mOfflineList.setVisible(true);
				try {
					FileUtils._mDiskLog.send({
						type : "offlinelist2_loadedlist"
					});
				} catch (G) {}

				break;
			case Page.MENU_NEW_BT_OFFLINE:
				break;
			case Page.MENU_PRINT:
				this.performSelectPrint();
				break;
			case Page.MENU_REVISION_LIST:
				this.performRevisionList();
				break;
			case Page.MENU_EDIT_PIC:
				this.performSelectionEditPic("box");
				break;
			}
		} else {
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				switch (B) {
				case Page.MENU_DOWNLOAD:
					return this.performSelectionDownload();
				case Page.MENU_TRANSFER:
					this.performSelectionTransfer();
					break;
				case Page.MENU_ALLFILEDOWNLOAD:
					this.performAllDownload();
					break;
				case Page.MENU_ALLFILETRANSFER:
					this.performAllTransfer();
					break;
				}
			} else {
				if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
					this.performSelectionCancelShare();
				} else {
					if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
						switch (B) {
						case Page.MENU_RESTORE:
							this.performSelectionRestore();
							return true;
						case Page.MENU_PERSIST_REMOVE:
							this.performSelectionRemoval();
							return true;
						case Page.MENU_CLEAR_ALL:
							this.performSelectionRemoval(true);
							return true;
						}
					}
				}
			}
		}
		return false;
	},
	installUploadFeature : function () {
		var _ = this;
		if ($("#barCmdUpload").length > 0) {
			if (disk.api.RestAPI.ACTIVEX_USE_UPGRADE_STRATEGY) {
				$.ajax({
					url : disk.api.RestAPI.ACTIVEX_UPGRADE + "&t=" + (new Date().getTime()),
					type : "GET",
					timeout : 3000,
					success : function (B, C) {
						var A = null;
						try {
							A = $.parseJSON(B);
							if (A.errorno == 0 && typeof A.content != "undefined") {
								A = A.content[0];
							}
							if (typeof A.release != "undefined" && typeof A.force != "undefined") {
								disk.ui.NativePluginFileUploader.forceUpgrade = parseInt(A.force);
								disk.ui.NativePluginFileUploader.upgradeVersion = A.release;
								disk.ui.UploadDialog.UPGRADE_TEXT = '<i class="up-dd-tip"></i><dfn class="txt1 tips-prx ellipsis">' + A.ext + '</dfn><a target="_blank" class="two-pix-btn" href="http://bs.baidu.com/netdisk/UploadPlugin_%s.exe">\u66f4\u65b0\u5b89\u88c5</a>';
							}
						} catch (D) {}

						finally {
							_._install0();
						}
					},
					error : function () {
						_._install0();
					}
				});
			} else {
				this._install0();
			}
		}
	},
	_install0 : function () {
		this._mUploadDialog = new disk.ui.UploadDialog({
				pane : $("#uploadDialog")[0],
				trigger : document.getElementById("barCmdUpload").parentNode,
				dndDropZone : $("#netdiskTips").parent().parent().get(0),
				dndSurroundingZone : document.documentElement,
				dndMarkContainer : document.body
			});
		this._mUploadDialog.install();
	},
	resolvePublicSharingIcon : function (A) {
		var A = FileUtils.getLocalCache()._mCache[0],
		_ = A[0].server_filename.match(/(.[^\.]+$)/);
		FileUtils.resolveFileSmallIcon(A[0].isdir, A, _ ? _[1] : "", $(".link-title .sprite-list-ic"), FileUtils.isInParticularDir(FileUtils.parseDirPath(A[0].path)) ? FileUtils.PART_KID_FOLDER : 0);
	},
	reportCDNOfPCS : function (A) {
		var _ = new Image();
		_.src = "http://dq.baidu.com/nocipher?pid=pcstest&ver=0&type=0&t=" + (new Date().getTime()) + "&success=" + A;
	},
	watchCDNOfPCS : function () {
		var _ = new Image(),
		A = this;
		_.onload = function () {
			A.reportCDNOfPCS(0);
		};
		_.onerror = function () {
			A.reportCDNOfPCS(1);
		};
		_.src = "http://cdn.baidupcs.com/monitor.jpg?xcode=1a81b0bbd448fc368d78cc336e28561a&t=" + (new Date().getTime());
	},
	newUserFromShareLink : function () {
		if (disk.getParam("from") == "share_activate_guide" || disk.getParam("from") == "share_notfound") {
			setTimeout(function () {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_SUCCESS,
					msg : "\u606d\u559c\u4f60\uff0c\u4f60\u662f\u767e\u5ea6\u4e91\u7f51\u76d8\u7684\u7528\u6237\u5566\uff0c5G\u6c38\u4e45\u514d\u8d39\u7a7a\u95f4\u5c5e\u4e8e\u4f60\u4e86\u3002",
					stiky : true,
					closeType : true
				});
			}, 2000);
		}
	},
	compilePingBack : function () {
		var B = document.referrer,
		_ = "fm_self",
		C = /(http|https)\:\/\/(tieba|hao123)\.baidu\.com/gi,
		A = /(http|https)\:\/\/www\.hao123\.com/gi;
		if (typeof B == "string" && B.length > 0) {
			if (C.test(B)) {
				_ = "fm_" + RegExp.$2;
			} else {
				if (A.test(B)) {
					_ = "fm_hao123";
				} else {
					if (B.indexOf("http://www.baidu.com/s?wd=") != -1) {
						_ = "fm_baidups";
					} else {
						if (Page.getCookie("BDSFRCVID").length > 0) {
							_ = "fm_alading";
						} else {
							_ = "fm_not_proved";
						}
					}
				}
			}
		}
		if (disk.DEBUG) {
			console.log("compiling ping back, resolve to ", _);
		}
		return _;
	},
	killFrame : function () {
		var _ = "frm",
		A = disk.getParam(_);
		if (top !== self && A != "hao123-iframe") {}

	},
	playMusicAll : function () {
		var F = this,
		A = FileUtils._mLocalCache.obtain('\u5206\u7c7b:"\u97f3\u4e50"');
		if (A.length == 0) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u5f53\u524d\u5217\u8868\u4e3a\u7a7a\uff0c\u8bf7\u6dfb\u52a0\u97f3\u4e50\u6587\u4ef6",
				sticky : false
			});
			return false;
		}
		var D = null;
		if (FileUtils._mViewMode == 0) {
			D = FileUtils._mInfiniteListView;
		} else {
			D = FileUtils._mInfiniteGridView;
		}
		if (D.isAllItemChecked()) {
			var C = [{
					songId : FileUtils._mMboxPlayerIndex++,
					songName : FileUtils.parseDirFromPath(A[0].path),
					link : Page.obtainDownloadURL(A[0])
				}
			];
			for (var B = 1, _ = A.length; B < _; B++) {
				C.push({
					songId : FileUtils._mMboxPlayerIndex++,
					songName : FileUtils.parseDirFromPath(A[B].path),
					link : Page.obtainDownloadURL(A[B])
				});
			}
			if (FileUtils._mMboxPlayer == null) {
				FileUtils._mMboxPlayer = new disk.ui.MusicBox(C);
			} else {
				if (!FileUtils._mMboxPlayer._mDisplay) {
					FileUtils._mMboxPlayer._reInit();
					FileUtils._mMboxPlayer.clearList();
					FileUtils._mMboxPlayer.setVisible(true);
					FileUtils._mMboxPlayer.addMusicData(C, true);
				} else {
					FileUtils._mMboxPlayer.clearList();
					FileUtils._mMboxPlayer.resetList(C, true);
				}
			}
			return false;
		}
		if (D._mCheckedChildren.length == 0) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_CAUTION,
				msg : "\u5f53\u524d\u9009\u62e9\u5217\u8868\u4e3a\u7a7a\uff0c\u8bf7\u9009\u62e9\u97f3\u4e50\u6587\u4ef6",
				sticky : false
			});
			return false;
		}
		var E = 0;
		for (_ = D._mCheckedChildren.length; E < _; E++) {
			if (D._mCheckedChildren[E] === true) {
				break;
			}
		}
		C = [{
				songId : FileUtils._mMboxPlayerIndex++,
				songName : FileUtils.parseDirFromPath(A[E].path),
				link : Page.obtainDownloadURL(A[E])
			}
		];
		for (B = E + 1, _ = A.length; B < _; B++) {
			if (D._mCheckedChildren[B] === true) {
				C.push({
					songId : FileUtils._mMboxPlayerIndex++,
					songName : FileUtils.parseDirFromPath(A[B].path),
					link : Page.obtainDownloadURL(A[B])
				});
			}
		}
		if (FileUtils._mMboxPlayer == null) {
			FileUtils._mMboxPlayer = new disk.ui.MusicBox(C);
		} else {
			if (!FileUtils._mMboxPlayer._mDisplay) {
				FileUtils._mMboxPlayer._reInit();
				FileUtils._mMboxPlayer.clearList();
				FileUtils._mMboxPlayer.setVisible(true);
				FileUtils._mMboxPlayer.addMusicData(C, true);
			} else {
				FileUtils._mMboxPlayer.clearList();
				FileUtils._mMboxPlayer.resetList(C, true);
			}
		}
	},
	main : function () {
		var G = this,
		F = $("#fileActionHeader");
		this.killFrame();
		var B = Page.getCookie("BDCLND");
		if (B && B.length > 0) {
			FileUtils.BDCLND_TOKEN = B;
		}
		this.newUserFromShareLink();
		this.installLogFeature();
		FileUtils.bindDownloadClient();
		if (Page.inViewMode(Page.VIEW_HOME)) {
			this._mToolbarMenuItems[Page.MENU_RENAME] = document.getElementById("barCmdRename");
			this._mToolbarMenuItems[Page.MENU_DELETE] = document.getElementById("barCmdDelete");
			this._mToolbarMenuItems[Page.MENU_SHARE] = document.getElementById("barCmdShareAll");
			this._mToolbarMenuItems[Page.MENU_PRINT] = document.getElementById("barCmdPrint");
			this._mToolbarMenuItems[Page.MENU_REVISION_LIST] = document.getElementById("barCmdRevision");
			this._mToolbarMenuItems[Page.MENU_COPY] = document.getElementById("barCmdCopy");
			this._mToolbarMenuItems[Page.MENU_MOVE] = document.getElementById("barCmdMove");
			this._mToolbarMenuItems[Page.MENU_UPLOAD] = document.getElementById("barCmdUpload");
			this._mToolbarMenuItems[Page.MENU_NEW_FOLDER] = document.getElementById("barCmdNewFolder");
			this._mToolbarMenuItems[Page.MENU_SELL_FILE_BIG] = document.getElementById("barCmdSellFile");
			this._mToolbarMenuItems[Page.MENU_DEVICE_OPEN] = document.getElementById("barCmdOpenDevice");
			this._mToolbarMenuItems[Page.MENU_BAR_SELL_FILE] = document.getElementById("barSellFile");
			this._mToolbarMenuItems[Page.MENU_ANOTHER_DOWNLOAD] = document.getElementById("barCmdDownload");
			this._mToolbarMenuItems[Page.MENU_PLAY_ALL] = document.getElementById("barCmdPlayAll");
			this._mToolbarMenuItems[Page.MENU_NEW_OFFLINE] = document.getElementById("barCmdNewOffline");
			this._mToolbarMenuItems[Page.MENU_OFFLINE_LIST] = document.getElementById("barCmdOffline");
			this._mToolbarMenuItems[Page.MENU_DEVICE] = document.getElementById("barPushToDevice");
			this._mToolbarMenuItems[Page.MENU_EDIT_PIC] = document.getElementById("barCmdEdit");
		} else {
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				this._mToolbarMenuItems[Page.MENU_DOWNLOAD] = document.getElementById("barCmdDownload");
				this._mToolbarMenuItems[Page.MENU_TRANSFER] = document.getElementById("barCmdTransfer");
				this._mToolbarMenuItems[Page.MENU_ALLFILEDOWNLOAD] = document.getElementById("barAllCmdDownload");
				this._mToolbarMenuItems[Page.MENU_ALLFILETRANSFER] = document.getElementById("barAllCmdTransfer");
			} else {
				if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
					this._mToolbarMenuItems[Page.MENU_CANCEL_SHARE] = document.getElementById("barCmdCancelShare");
				} else {
					if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
						this._mToolbarMenuItems[Page.MENU_RESTORE] = document.getElementById("barCmdRestore");
						this._mToolbarMenuItems[Page.MENU_PERSIST_REMOVE] = document.getElementById("barCmdPersistDelete");
						this._mToolbarMenuItems[Page.MENU_CLEAR_ALL] = document.getElementById("barCmdPersistClear");
					}
				}
			}
		}
		if ($.browser.msie) {
			if ($.browser.version == 6) {
				$('a[href="javascript:;"]').click(function (_) {
					_.preventDefault();
				});
				$('a[href="javascript:;"]').live("click", function (_) {
					_.preventDefault();
				});
			}
		}
		if (!Page.inViewMode(Page.VIEW_PROPERTY_OTHER) && !Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
			FileUtils.sendUserReport(this.compilePingBack());
		}
		if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
			this.resolvePublicSharingIcon();
			$("#cancelButton").click(function () {
				FileUtils.cancelShareInView({
					shareid_list : $.stringify([FileUtils.share_id])
				});
			});
			if (encodeURI(disk.getParam("third"))) {
				u_href = $("a.link-usern").attr("href") + "&third=" + encodeURI(disk.getParam("third"));
				$("a.link-usern").attr("href", u_href);
			}
		}
		if (Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) || Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
			$(document).ready(function () {
				FileUtils.onPersonalPageTabChange("share");
			});
		}
		var C = function (B) {
			if (this.className.indexOf("disabled") != -1) {
				return;
			}
			var A = this.getAttribute("_i");
			if (A == null) {
				throw new Error("Menu Not Registered");
			}
			A = parseInt(A);
			var _ = G.dispatchActionPerformed(A, this, B);
			if (!_) {
				if (disk.DEBUG) {
					console.log("Suppress a download event, hand it over");
				}
				return false;
			}
			return true;
		};
		for (var A = 0, _ = this._mToolbarMenuItems.length; A < _; A++) {
			if (this._mToolbarMenuItems[A] == null) {
				continue;
			}
			$(this._mToolbarMenuItems[A]).attr("_i", A);
			$(this._mToolbarMenuItems[A]).click(C);
		}
		$("#barDropdownTriggerMore").mouseenter(function () {
			$(this).parents("p:first").siblings(".pull-down-menu").css("left", $(this).offset().left - $("#fileActionHeader").offset().left + parseInt($("#fileActionHeader").parent().css("marginLeft"), 10)).show();
			$(this).parents("p:first").siblings(".pull-down-menu").find("a").css("text-align", "center");
		}).mouseleave(function () {
			$(this).parents("p:first").siblings(".pull-down-menu").hide();
		});
		$("#barPushToDeviceMore").mouseenter(function (_) {
			$(this).parents("p:first").siblings(".device-menu").css({
				left : $(this).offset().left - $("#fileActionHeader").offset().left + parseInt($("#fileActionHeader").parent().css("marginLeft"), 10),
				top : 33
			}).show();
			Page.obtain().installDevice();
		}).mouseleave(function () {
			$(this).parents("p:first").siblings(".device-menu").hide();
		});
		$("#barCmdOpenDevice").mouseenter(function (_) {
			if ($(this).find("ul").length) {
				$(this).find("ul").show();
			} else {
				$(this).append('<ul class="device-bar-more"><li><a alt="pushDevice" href="javascript:;">\u63a8\u9001\u4efb\u52a1\u5217\u8868</a></li><li><a alt="addDevice" href="javascript:;" >\u6dfb\u52a0\u8bbe\u5907</a></li></ul>');
				$(this).find("ul").show();
			}
		}).mouseleave(function () {
			$(this).find("ul").hide();
		}).bind({
			click : function (A) {
				var _ = A.target.getAttribute("alt");
				switch (_) {
				case "pushDevice":
					Page.obtain().performOpenDevice();
					break;
				case "addDevice":
					Page.obtain().performAddDevice();
					break;
				default:
					break;
				}
			}
		});
		$("#fileActionHeader .pull-down-menu,#fileActionHeader .device-menu").mouseenter(function () {
			$(this).show();
		}).mouseleave(function () {
			$(this).hide();
		});
		$("#barCmdPublic, #barCmdMore, #userActions, #barUserInfo").hover(function () {
			if ($(this).hasClass("disabled") == false) {
				$(this).addClass("b-img-over");
			}
		}, function () {
			$(this).removeClass("b-img-over");
		});
		$(".abtn,.bbtn,.cbtn,.ibtn,.ubtn,.robin-b-ic-query").click(function () {
			this.blur();
		});
		var D = function () {
			if (FileUtils.isListViewLocked()) {
				return;
			}
			if (FileUtils.getViewMode() != FileUtils.VIEW_LIST) {
				return;
			}
			if (Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE) || Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN)) {
				FileUtils._shareDropdownDrawerTrigger(null, true, 200);
			}
			if (FileUtils._mPrecedeSortHandler == null || FileUtils._mPrecedeSortHandler != this) {
				$(".indicator").removeClass("desc").removeClass("asc");
			}
			$(this).toggleClass("desc");
			var _ = $(this).hasClass("desc");
			if (_) {
				$(this).removeClass("asc");
			} else {
				$(this).addClass("asc");
			}
			if (this.id.indexOf("time") != -1) {
				FileUtils.sortBy(FileUtils.SORT_BY_TIME, _);
			} else {
				if (this.id.indexOf("size") != -1) {
					FileUtils.sortBy(FileUtils.SORT_BY_SIZE, _);
				} else {
					if (this.id.indexOf("remaining") != -1) {
						FileUtils.sortBy(FileUtils.SORT_BY_REMAINING, _);
					} else {
						if (this.id.indexOf("vCnt") != -1) {
							FileUtils.sortBy(FileUtils.SORT_BY_VCNT, _);
						} else {
							if (this.id.indexOf("tCnt") != -1) {
								FileUtils.sortBy(FileUtils.SORT_BY_TCNT, _);
							} else {
								if (this.id.indexOf("dCnt") != -1) {
									FileUtils.sortBy(FileUtils.SORT_BY_DCNT, _);
								} else {
									FileUtils.sortBy(FileUtils.SORT_BY_NAME, _);
								}
							}
						}
					}
				}
			}
			FileUtils._mPrecedeSortHandler = this;
			FileUtils.getLocalCache().markDirty(true);
			return false;
		};
		if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
			var E;
			if ((E = disk.getParam("view")) && E !== "share") {}
			else {
				FileUtils._mSortTriggers = [document.getElementById("nameCompareTrigger"), document.getElementById("timeCompareTrigger"), document.getElementById("vCntCompareTrigger"), document.getElementById("tCntCompareTrigger"), document.getElementById("dCntCompareTrigger")];
				FileUtils._mSortTriggers[0].onclick = FileUtils._mSortTriggers[1].onclick = FileUtils._mSortTriggers[2].onclick = FileUtils._mSortTriggers[3].onclick = FileUtils._mSortTriggers[4].onclick = D;
			}
		} else {
			if (document.getElementById("remainingCompareTrigger")) {
				FileUtils._mSortTriggers = [document.getElementById("nameCompareTrigger"), document.getElementById("sizeCompareTrigger"), document.getElementById("timeCompareTrigger"), document.getElementById("remainingCompareTrigger")];
				FileUtils._mSortTriggers[0].onclick = FileUtils._mSortTriggers[1].onclick = FileUtils._mSortTriggers[2].onclick = FileUtils._mSortTriggers[3].onclick = D;
			} else {
				FileUtils._mSortTriggers = [document.getElementById("nameCompareTrigger"), document.getElementById("sizeCompareTrigger"), document.getElementById("timeCompareTrigger")];
				FileUtils._mSortTriggers[0].onclick = FileUtils._mSortTriggers[1].onclick = FileUtils._mSortTriggers[2].onclick = D;
			}
		}
		if (Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
			FileUtils._mSortTriggers.push(document.getElementById("remainingCompareTrigger"));
			FileUtils._mSortTriggers[3].onclick = D;
		}
		$("#module-path").click(function (_) {
			if (FileUtils._mQuerying) {
				return false;
			}
			if (FileUtils.isListViewLocked()) {
				return false;
			}
			disk.util.HistoryManager.getDefault().navigate(FileUtils.MODULE_DIR, null);
			_.preventDefault();
			_.stopPropagation();
			return false;
		});
		$("#module-type").click(function (_) {
			if (FileUtils._mQuerying) {
				return false;
			}
			if (FileUtils.isListViewLocked()) {
				return false;
			}
			disk.util.HistoryManager.getDefault().navigate(FileUtils.MODULE_CATEGORY, null);
			_.preventDefault();
			_.stopPropagation();
			return false;
		});
		$("#barCmdViewList").click(function () {
			if (FileUtils.isListViewLocked()) {
				return;
			}
			FileUtils.FLAG_USER_CHANGED_MODE = true;
			FileUtils.switchViewMode(FileUtils.VIEW_LIST);
			FileUtils.USER_VIEW_MODE = FileUtils.VIEW_LIST;
			return false;
		});
		$("#barCmdViewSmall").click(function () {
			if (FileUtils.isListViewLocked()) {
				return;
			}
			FileUtils.FLAG_USER_CHANGED_MODE = true;
			FileUtils.switchViewMode(FileUtils.VIEW_GRID_ICON);
			FileUtils.USER_VIEW_MODE = FileUtils.VIEW_GRID_ICON;
			return false;
		});
		disk.util.HistoryManager.getDefault().listen(FileUtils.MODULE_SEARCH, function (A, _) {
			if (typeof A.key == "undefined") {
				A.key = "";
			}
			if (Page.inViewMode(Page.VIEW_HOME)) {
				$("#tab-home").addClass("on");
				$("#aside-menu a.b-no-ln").removeClass("on");
			}
			$("#barCmdPlayAll").addClass("none");
			if (!_) {
				$("#barCmdNewFolder").addClass("none");
			}
			G.performGlobalSearch(A.key, _);
			FileUtils.setModule(FileUtils.MODULE_DIR);
			if ((FileUtils.getViewMode() != FileUtils.USER_VIEW_MODE)) {
				FileUtils.switchViewMode(FileUtils.USER_VIEW_MODE);
			}
		});
		disk.util.HistoryManager.getDefault().listen(FileUtils.MODULE_NOTI, function (A, _) {
			if (_ === true) {
				return;
			}
		});
		disk.util.HistoryManager.getDefault().listen(FileUtils.MODULE_CATEGORY, function (A, _) {
			if (disk.DEBUG) {
				console.log(">>>>>>>>>switch to category");
			}
			if (FileUtils.isListViewLocked()) {
				return;
			}
			if (_ === true) {
				return;
			}
			if (typeof A.type == "undefined") {
				A.type = "";
			} else {
				$(".b-list-3 a", "#aside").removeClass("on");
				$(".b-list-3 a[cat=" + A.type + "]").addClass("on");
			}
			if (!_) {
				$("#barCmdNewFolder").addClass("none");
			}
			G.performCategory(A.type);
			FileUtils.setModule(FileUtils.MODULE_CATEGORY);
		});
		$(".type-a-mus").click(function (_) {
			Page.obtain().performType(0);
			_.preventDefault();
			return false;
		});
		$(".type-a-pic").click(function (_) {
			if ($(this).attr("href") != "/disk/timeline") {
				Page.obtain().performType(1);
			} else {
				window.location.href = "/disk/timeline";
			}
			_.preventDefault();
			return false;
		});
		$(".type-a-mov").click(function (_) {
			Page.obtain().performType(2);
			_.preventDefault();
			return false;
		});
		$(".type-a-doc").click(function (_) {
			Page.obtain().performType(3);
			_.preventDefault();
			return false;
		});
		$(".type-a-app").click(function (_) {
			Page.obtain().performType(6);
			_.preventDefault();
			return false;
		});
		$(".type-a-oth").click(function (_) {
			Page.obtain().performType(5);
			_.preventDefault();
			return false;
		});
		Page.changeUnionLogin();
		this.watchCDNOfPCS();
	},
	installAwardIntrDialog : function () {
		var _ = localStorage.getItem(FileUtils.AWARD_DIALOG_INSTALL + "_" + FileUtils.sysUID.replace(/@/g, ""));
		if (_) {
			return;
		}
		$.getScript("/res/static/js/buss/award_intr.js?t=201404241528", function () {
			if (typeof disk.ui.AwardIntr == "undefined") {
				if (disk.DEBUG) {
					console.log("loaded awardIntr dialog failed");
				}
				return;
			} else {
				$.ajax({
					type : "POST",
					timeout : 3000,
					url : disk.api.RestAPI.TASK_QUERY + "?detail=1&t=" + (new Date().getTime()),
					dataType : "json",
					success : function (_) {
						if (_ && _.errno == 0) {
							FileUtils.showAwardIntrDialog(_);
						} else {
							return false;
						}
					}
				}).error(function () {
					return false;
				});
			}
		});
	},
	installSellDialog : function (A) {
		if (FileUtils.third != 0) {
			if (typeof disk.util.bindBDPass == "undefined") {
				ark.use({
					url : ["/ccres/static/js/buss/bindBDPass.js", "/ccres/static/css/bind_pass.css"],
					noDependency : false,
					enforceUrl : true
				}, function (_) {
					disk.util.bindBDPass.bindDialog();
				});
			} else {
				disk.util.bindBDPass.bindDialog();
			}
			return;
		}
		function _() {
			var B = new disk.ui.SellDialog();
			B.setVisible(true);
			if (A) {
				return;
			}
			var _ = FileUtils.getListViewCheckedItems();
			B.pushFileList(_);
		}
		if (!disk.ui.SellDialog) {
			Utilities.useToast({
				toastMode : disk.ui.Toast.MODE_LOADING,
				position : disk.ui.Panel.TOP,
				msg : "\u8d44\u6e90\u52a0\u8f7d\u4e2d&hellip;",
				sticky : false
			});
			ark.use({
				url : ["/ppres/static/css/send_box.css", "/ppres/static/js/buss/send_box.js", "/res/static/css/sell_dialog.css", "/res/static/js/util/form_helper.js", "/res/static/js/buss/dialog/yun_sell_dialog_tpl.js", "/res/static/js/buss/dialog/sell_dialog.js"],
				noDependency : true,
				enforceUrl : true
			}, function () {
				disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
				_();
			});
		} else {
			_();
		}
	},
	installDevice : function () {
		var A = FileUtils.getListViewCheckedItems(),
		B = function () {
			if (_ + 1) {
				clearTimeout(_ + 1);
			}
			if (FileUtils._mPushDevice == null) {
				ark.use({
					url : ["/buss/dialog/push_device_dialog.js", "/dialog/push-yun-device-dialog.css"],
					noDependency : false
				}, function () {
					if (typeof disk.ui.PushDeviceDialog == "function") {
						FileUtils._mPushDevice = new disk.ui.PushDeviceDialog();
						FileUtils._mPushDevice.build(A);
					} else {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u8bbe\u5907\u63a8\u9001\u52a0\u8f7d\u5931\u8d25\uff0c\u8bf7\u5237\u65b0\u91cd\u8bd5",
							sticky : false
						});
					}
				});
			} else {
				FileUtils._mPushDevice.rebuild(A);
			}
		},
		_ = setTimeout(function () {
				B();
			}, 600);
	},
	installBTDialog : function () {
		$.getScript("/res/static/js/buss/dialog/createoffline_bt_dialog.js?t=201404241528", function () {
			if (typeof disk.ui.ResolveBTDialog == "undefined") {
				if (disk.DEBUG) {
					console.warn("loaded ResolveBTDialog failed");
				}
				return;
			} else {
				if (FileUtils.getFlagHaveBTPlugs()) {
					if (FileUtils.bt_paths && FileUtils.bt_paths.length > 0 && Page.inViewMode(Page.VIEW_HOME)) {
						if (disk.DEBUG) {
							console.log("btpaths: ", FileUtils.bt_paths);
						}
						FileUtils.bt_paths = FileUtils.bt_paths.replace(/&quot;/g, '"');
						var _ = null;
						try {
							_ = $.parseJSON(FileUtils.bt_paths);
						} catch (A) {
							if (disk.DEBUG) {
								console.warn("bt_paths error");
							}
						}
						if (disk.DEBUG) {
							console.log(_);
						}
						if (_ && (_ instanceof Array) && _.length > 0) {
							if (disk.DEBUG) {
								console.log("start resolve bt list");
							}
							disk.ui.ResolveBTList.init(_);
						}
					}
				}
			}
		});
	},
	installLogFeature : function () {
		FileUtils._mDiskLog = new disk.FELog({
				url : disk.api.RestAPI.ANALYTICS,
				logObjectName : "fe_log",
				baseParam : {
					page : Page.inViewMode(),
					clienttype : 0
				},
				sendBaseParamTag : false
			});
	}
};
(function () {
	var _ = function () {
		this._mTabIndicators = [];
		this._mTabContents = [];
		this._mSelectedIndex = -1;
		this._mOnClass = "on";
		var _ = this;
		this._mTabClickListener = function () {
			var A = parseInt(this.getAttribute("_idx"));
			_.setSelected(A);
			return false;
		};
	};
	Utilities.simulateFocus = function (G, C, _) {
		var B = $("#" + G + " input"),
		D = $("#" + G),
		A = function (_) {
			D.addClass("b-input-focus");
			D.addClass("b-holder-off");
		},
		F = function () {
			D.removeClass("b-input-focus");
			if (B.attr("value").trim().length == 0) {
				D.removeClass("b-holder-off");
			}
		};
		B.focus(A);
		B.blur(F);
		if (typeof C != "undefined") {
			var E = document.getElementById(C);
			if (E) {
				$(E).click(function (_) {
					D.addClass("b-holder-off");
				});
				if (B.attr("value").trim().length > 0) {
					D.addClass("b-holder-off");
					B.focus();
				}
			}
		}
	};
})();
(function () {
	window.onload = function () {
		var _ = $(".jfk-scrollbar");
		fn = function (B) {
			var A = document.createElement("div"),
			_ = document.createElement("div");
			A.setAttribute("class", "scrollbar-shadow scrollbar-top-shadow");
			_.setAttribute("class", "scrollbar-shadow scrollbar-bottom-shadow");
			var C = $(B).parent();
			C.append(A);
			C.append(_);
			$(window).resize(function () {
				setTimeout(function () {
					var A = B.scrollTop,
					_ = B.scrollHeight - B.clientHeight;
					$(".scrollbar-top-shadow", C).css("opacity", _ ? A / 0.2 / _ : 0);
					$(".scrollbar-bottom-shadow", C).css("opacity", _ ? (_ - A) / 0.2 / _ : 0);
				}, 100);
			});
			$(B).scroll(function () {
				var A = $(this).parent(),
				B = this.scrollTop,
				_ = this.scrollHeight - this.clientHeight;
				$(".scrollbar-top-shadow", A).css("opacity", _ ? B / 0.2 / _ : 0);
				$(".scrollbar-bottom-shadow", A).css("opacity", _ ? (_ - B) / 0.2 / _ : 0);
			});
		};
		if (!(l = _.length) || $.browser.msie) {
			return;
		} else {
			var A = 0;
			for (; A < l; A++) {
				var B = _[A];
				fn(B);
			}
		}
	};
})();
(function () {
	var _ = function (A) {
		this.target = A.target;
		this.topOffset = A.topOffset || 0;
		this.imgSrc = A.imgSrc;
		this.imgTit = A.imgTit || "";
		this._targetId = disk.obtainId();
		this.flagAddIframe = A.flagAddIframe || false;
		this.imgDes = A.imgDes || _.Templates.bottom_msg;
		this.clickUrl = A.clickUrl || _.Templates.click_url;
		this.panl = null;
		this.arrowPos = A.arrowPos || "top";
		this.changetextdec = A.changetextdec || false;
		this.showsource = A.showsource || "";
		this.hoverCallBack = A.hoverCallBack || null;
		this.leaveCallBack = A.leaveCallBack || null;
		this.panlclick = A.panlclick;
		disk.ui.Panel.call(this, this._prebuild());
		this._init();
	};
	_.iframeId = disk.obtainId();
	_.getIframeObj = function () {
		var A = $("#" + _.iframeId);
		if (!A.length) {
			$(document.body).append("<iframe src='about:blank' id='" + _.iframeId + "' class='iframe_two_dimension_code' frameborder='0'></iframe>");
			A = $("#" + _.iframeId);
		}
		return A;
	};
	_.Templates = {
		bottom_msg : this.imgDes || "\u626b\u63cf\u6216\u70b9\u51fb\u4e8c\u7ef4\u7801\u7acb\u5373\u5b89\u88c5",
		main_html : function (D, B, C, A, _, E) {
			return '<div id="' + D + '" class="dialog_two_dimension_code"><div class="inner_two_dimension_code"><div class="two_dimension_code_' + E + (E == "down" ? " rotate-3" : "") + '"></div><div title="' + A + '" class="two_dimension_code_main" ></div></div></div>';
		},
		click_url : "http://pan.baidu.com/download"
	};
	_._FELog = null;
	_.prototype = new disk.ui.Panel();
	_.prototype._prebuild = function () {
		var D = this;
		$(document.body).append(_.Templates.main_html(D._targetId, D.imgSrc, D.imgDes, D.imgTit, D.bottomMsg, D.arrowPos));
		D.panl = $("#" + D._targetId);
		var B = function (_) {
			if (_.complete == true || _.readyState == "complete" || _.readyState == "loaded") {
				D.panl.find(".two_dimension_code_main").html("").append(_).find("img").css("marginTop", 10);
				D.panl.find(".inner_two_dimension_code").append('<div class="two_dimension_code_bottom">' + D.imgDes + "</div>");
			}
		},
		C = function () {
			D.panl.find(".two_dimension_code_main").html("\u751f\u6210\u5931\u8d25\uff0c<a href='javascript:;' class='retryBtn'>\u91cd\u8bd5\u4e00\u4e0b</a>");
			D.panl.find(".retryBtn").click(function () {
				A();
			});
		},
		A = function () {
			D.panl.find(".two_dimension_code_main").html("\u6b63\u5728\u751f\u6210\u4e8c\u7ef4\u7801\u2026\u2026");
			D.panl.find(".two_dimension_code_bottom").remove();
			var _ = new Image();
			_.src = D.imgSrc || "";
			_.onload = function () {
				B(_);
			};
			_.onerror = function () {
				C(_);
			};
		};
		A();
		return {
			pane : D.panl[0]
		};
	};
	_.prototype.beforeInit = function () {
		if (!_._FELog) {
			if (FileUtils && FileUtils._mDiskLog) {
				_._FELog = FileUtils._mDiskLog;
			} else {
				_._FELog = new disk.FELog({
						url : disk.api.RestAPI.ANALYTICS,
						logObjectName : "fe_log",
						baseParam : {
							clienttype : 0
						},
						sendBaseParamTag : false
					});
			}
		}
	};
	_.prototype._init = function () {
		var B = this;
		B.beforeInit();
		B.target.mouseover(function () {
			if (!B.changetextdec) {
				B.changeUnderLine(true);
			}
			B.setVisible(true);
		}).mouseout(function () {
			B.changeUnderLine(false);
			B.setVisible(false);
		});
		B.panl.mouseover(function () {
			if (!B.changetextdec) {
				B.changeUnderLine(true);
			}
			if (typeof B.hoverCallBack == "function") {
				B.hoverCallBack();
			}
			B.setVisible(true);
		}).mouseleave(function () {
			B.changeUnderLine(false);
			B.setVisible(false);
			if (typeof B.leaveCallBack == "function") {
				B.leaveCallBack();
			}
		});
		var A = function () {
			var A = {
				type : "twodimensioncode",
				clienttype : "0",
				currentUrl : encodeURIComponent(window.location.href)
			};
			_._FELog.send(A);
			window.open(B.clickUrl);
		};
		if (!B.panlclick) {
			B.panl.click(function () {
				A();
			});
			B.target.click(function () {
				A();
			});
		}
		$("#retryBtn").click(function () {
			retry();
		});
	};
	_.prototype.changeUnderLine = function (A) {
		var C = this,
		_ = A ? "underline" : "none",
		B = C.target;
		if (B.attr("tagName") == "A") {
			B.css("textDecoration", _);
		} else {
			B.find("a").css("textDecoration", _);
		}
	};
	_.prototype.getPosi = function () {
		var E = this,
		D = E.target.width() + parseInt(E.target.css("paddingLeft"), 10),
		A = E.target.height(),
		C = E.target.offset(),
		B = C.left,
		_ = C.top + E.topOffset + 21;
		B += (D - E.panl.width());
		return {
			x : B,
			y : _
		};
	};
	_.prototype.setVisible = function (B) {
		var C = this,
		A;
		if (B) {
			A = this.getPosi();
			C.setLocation(A.x, A.y);
		}
		if (C.arrowPos == "down") {
			$(".dialog_two_dimension_code").css("paddingBottom", "9px");
			$(".iframe_two_dimension_code").css("height", "200px");
		}
		if (C.flagAddIframe) {
			_.getIframeObj().css(B ? {
				left : A.x,
				top : A.y,
				zIndex : 888,
				display : "block"
			}
				 : {
				display : "none"
			});
		}
		disk.ui.Panel.prototype.setVisible.call(C, B);
	};
	_.prototype.addAnalytics = function () {};
	disk.ui.TwoDimension = _;
})();
(function (_) {
	var A = function () {
		A.superclass.constructor.call(this);
		this._mHomeFlags = 0;
		this._mLogImg = null;
		this._mTimestamp = null;
		this._mFM = null;
		this._mFMVisibility = null;
		this._mFMNativeIntstall = true;
		this._mViewFlags = 0;
		this._mDragSurvallance = null;
	};
	A.TAG = "HomePageWindow";
	A.PATTERN_TIMESTAMP = /\?t=(\d+)/gi;
	A.FLAG_INITIAL_CHANCE = 1;
	A.DRAG_SELECTION_LISTVIEW_READY = 2;
	A.DRAG_SELECTION_REQUIREMENT_READY = 4;
	A.FEATURE_LETENCY = _.st.ClassLoader.getQulifier();
	A.FEATURE_LETENCY_PATH = ["ui/93432632fd958a7d55fe93889344c597"];
	A.FEATURE_MUSIC_PLAYER = _.st.ClassLoader.getQulifier();
	A.FEATURE_MUSIC_PLAYER_2 = _.st.ClassLoader.getQulifier();
	A.FEATURE_MUSIC_PLAYER_PATH = ["util/music_box"];
	A.ARGUMENT_APPLICATION_CONFIG = 2;
	A.ARGUMENT_INITIAL_LIST = 3;
	A.TAG_LISTVIEW = 0;
	A.TAG_GRIDVIEW = 1;
	A.FLAG_SHOW_VIP_DIALOG = 0;
	_.extend(A, _.st.AbstractPageWindow, {
		_init : function (_) {
			FileUtils.buildDirMgr();
			Utilities.resolveLayout();
			Utilities.simulateFocus("search", "qPlaceHolder");
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				var C = $.parseJSON(_);
				if (C === null) {
					C = [];
				}
				FileUtils.getLocalCache().add(FileUtils.ROOT_ID, C);
				if (C && C.length != 0) {
					FileUtils.onBeforeRenderingListView(C);
				}
				FileUtils.load(FileUtils.ROOT_ID);
			}
			Page.obtain().main();
			var B = disk.util.HistoryManager.interpretCurrentWindow(true);
			if (!Page.inViewMode(Page.VIEW_HOME)) {
				if (B != null) {
					FileUtils.tryToUseHidingStrategy(B);
				}
			}
			disk.util.HistoryManager.interpretCurrentWindow();
			if (B != null && (!Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE))) {
				var A = B[1].key;
				if (B[0] == "s" && typeof A != "undefined") {
					disk.ui.header.setSearchFormValue(A);
					FileUtils._mLastQueryTerms = A;
				}
				FileUtils.FLAG_COMING_BY_SEARCH = 1;
			} else {
				FileUtils.issueProxyLoading(true);
				FileUtils.loadDir(FileUtils.ROOT_ID);
			}
			Utilities.scheduleQueryQuota(function () {
				Utilities.isOverdue();
				Utilities.isFullAward();
			}, 1, 1);
			document.body.setAttribute("_title", document.title);
		},
		_prepend0 : function () {
			var _ = this;
			ark.use({
				url : ["disk.buss.hotrec", "disk.buss.share_home", "disk.buss.clientdownload"],
				noDependency : true
			}, function () {
				_.log(A.TAG, "hotrec and share_home done");
				if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
					disk.util.Hotrec.buttonClickRPLog(FileUtils.shareLogData);
				}
			});
		},
		onStart : function (_) {
			this.dispatchWindowStarted();
			disk.ui.Utilities.execOptionQuery();
			var B = null,
			C = "";
			if ((B = $("link").get(1))) {
				C = B.getAttribute("href");
			}
			A.PATTERN_TIMESTAMP.lastIndex = 0;
			C = A.PATTERN_TIMESTAMP.test(C) ? RegExp.$1 : new Date().getTime();
			this._mTimestamp = C;
			ark.conf({
				baseUrl : ["/res/static/js/", "/res/static/css/"],
				exports : "disk",
				timestamp : this._mTimestamp,
				maxInlineDependency : 20
			});
			ark.setDebugEnabled(disk.DEBUG);
			this._prepend0();
			this.log(A.TAG, "HomePageWindow started, with argument " + _);
			_ = this.getArgumentAt(A.ARGUMENT_INITIAL_LIST);
			this._init(_);
			this.onInitialRequestFeature();
			var D = this;
			$("#barCmdPlayAll").click(function (_) {
				D.requestFeature(A.FEATURE_MUSIC_PLAYER, A.FEATURE_MUSIC_PLAYER_PATH);
				return false;
			});
			if (Page.inViewMode(Page.VIEW_HOME)) {
				ark.use("disk.ui.FeatureManual", "disk.util.PageVisibility", {
					url : "disk.util.storage",
					noDependency : true
				}, "feature_manual.css", function (C) {
					if (disk.getParam("frm") == "sharebt") {
						return;
					}
					var F = D._getWizardKey(),
					_ = localStorage.getItem(F);
					A.FLAG_SHOW_VIP_DIALOG = _;
					if (_) {
						return;
					}
					if (FileUtils.is_vip && !_) {
						D._installFM();
					} else {
						var E = new disk.util.PageVisibility(),
						B = E.listen(function (A) {
								if (A === false) {
									var _ = D._getVIPWizardKey(),
									B = localStorage.getItem(_);
									if (!B) {
										return;
									}
									D._installFM();
								}
							}, true);
						if (disk.DEBUG) {
							console.log("using native install ", B);
						}
						D._mFMVisibility = E;
						D._mFMNativeIntstall = B;
						if (B === false) {
							D.setMessageLoopEnabled(true);
						}
					}
				});
			}
		},
		onMessageLoop : function () {
			if (this._mFMNativeIntstall === false) {
				if (disk.DEBUG) {
					console.log("loop native install");
				}
				var _ = this._getVIPWizardKey(),
				A = localStorage.getItem(_);
				if (A) {
					this._installFM();
					this._mFMNativeIntstall = true;
				}
			}
		},
		_getWizardKeyPrefix : function () {
			return FileUtils.sysUID.replace(/@/gi, "");
		},
		_getWizardKey : function () {
			return this._getWizardKeyPrefix() + "_vip_wizard_display";
		},
		_getVIPWizardKey : function () {
			return "vip_" + this._getWizardKeyPrefix();
		},
		_installFM : function () {
			if (this._mFM) {
				return;
			}
			var _ = new disk.ui.FeatureManual({
					stepTargets : [null, $(".top-username").parent().get(0), $(".type-a-pic").get(0), $("#barCmdOffline").get(0), $("#awardBubbleP").get(0), null],
					closeOnExceed : true
				});
			_.onStepEvent = function (F, A, C, _, B, E) {
				switch (F) {
				case 0:
					break;
					var D = new Image();
					D.src = "/res/static/images/feature/fm_1.png";
				case 1:
					E[0] = E[0] + 40;
					D = new Image();
					D.src = "/res/static/images/feature/fm_2.png";
					break;
				case 2:
					E[0] = E[0] + B.offsetWidth;
					E[1] = E[1] - B.offsetHeight - 60;
					D = new Image();
					D.src = "/res/static/images/feature/fm_3.png";
					break;
				case 3:
					E[0] = E[0] - 80;
					D = new Image();
					D.src = "/res/static/images/feature/fm_4.png";
					break;
				case 4:
					E[0] = E[0] + B.offsetWidth;
					E[1] = E[1] + 76;
					D = new Image();
					D.src = "/res/static/images/feature/fm_5.png";
					break;
				case 5:
					break;
				}
			};
			_.start();
			this._mFM = _;
			if (this._mFMVisibility) {
				this._mFMVisibility.unlisten();
			}
			var A = this._getWizardKey();
			localStorage.setItem(A, "1");
		},
		onInitialRequestFeature : function () {
			this.log(A.TAG, "initial feature request");
			this.requestFeature(A.FEATURE_LETENCY, A.FEATURE_LETENCY_PATH);
		},
		_initVideo : function () {
			if (typeof disk.ui.VideoUtils === "undefined") {
				return;
			}
			var _ = disk.ui.VideoUtils;
			setTimeout(function () {
				_ && _.processNewComing();
			}, 100);
		},
		_installCommentPlugin : function (_) {
			var A = this;
			if (typeof disk.ui.Comment == "undefined") {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u52a0\u8f7d\u7ec4\u4ef6\u4e2d\uff0c\u8bf7\u7a0d\u5019...",
					sticky : true
				});
				ark.use({
					url : ["/ppres/static/css/comment_panel.css", "/ppres/static/js/buss/comment_panel.js"],
					noDependency : false,
					enforceUrl : true
				}, function () {
					if (typeof _ == "function") {
						Utilities.useCloseToast();
						_();
					}
				});
			}
		},
		_installInWhiteList : function () {
			var _ = disk.Context.getService(disk.Context.SERVICE_DIALOG);
			_.setVisible(true);
			_.setMessage("\u8bf7\u8f93\u5165\u63cf\u8ff0", "<div class='in-white-user-list-tips'><textarea maxlength=500 style='border: solid 1px #ccc;font-size: 14px;padding: 5px;resize: none;' id='in-white-user-list-txt' rows='5' cols='45' placeholder='\u8bf7\u8f93\u5165...'>" + FileUtils.whiteListShareInfo + "</textarea><div>", function () {
				var _ = $("#in-white-user-list-txt"),
				B = this,
				A = $.trim(_.val());
				if (A.length) {
					if (A.length > 500) {
						Utilities.useToast({
							toastMode : disk.ui.Toast.MODE_CAUTION,
							msg : "\u63cf\u8ff0\u4fe1\u606f\u8fc7\u957f\uff0c\u8bf7\u63a7\u5236\u5728500\u5b57\u4ee5\u5185",
							sticky : false
						}).setGravity(disk.ui.Panel.TOP);
						return;
					}
					$.getJSON("/share/updateshareinfo", {
						shareid : FileUtils.share_id,
						uk : FileUtils.sysUK,
						shareinfo : A
					}, function (_) {
						if (_.errno == 0) {
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_SUCCESS,
								msg : "保存成功",
								sticky : false
							}).setGravity(disk.ui.Panel.TOP);
							FileUtils.whiteListShareInfo = A;
							B.setVisible(false);
						} else {
							Utilities.useToast({
								toastMode : disk.ui.Toast.MODE_CAUTION,
								msg : "保存失败，请稍候重试",
								sticky : false
							}).setGravity(disk.ui.Panel.TOP);
						}
					});
				} else {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u63cf\u8ff0\u4fe1\u606f\u4e0d\u80fd\u4e3a\u7a7a",
						sticky : false
					}).setGravity(disk.ui.Panel.TOP);
					_.focus();
				}
			}, function () {
				this.setVisible(false);
			});
		},
		_onPageViewLetencyStart : function () {
			var C = this;
			if (Page.inViewMode(Page.VIEW_HOME)) {
				Page.obtain().installBTDialog();
			}
			if (Page.inViewMode(Page.VIEW_HOME) || Page.inViewMode(Page.VIEW_OWN) || Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
				Page.obtain().installUploadFeature();
			}
			this.sendRequirementReady();
			if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME) || Page.inViewMode(Page.VIEW_RECYCLE_BIN)) {
				disk.Context.requestFeature(disk.Context.FEATURE_STORAGE, {
					succ : function () {
						FileUtils.getFlagTimeLineByCookie();
					}
				});
			}
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				var A = $(".sh-cnt-ct"),
				_ = A.find(".funcs-comment");
				if (_.length) {
					_.click(function () {
						var B = function (C) {
							var D = $(C.target);
							if (D.hasClass("funcs-comment") || D.hasClass("page-number")) {
								return;
							}
							var _ = D.parents(".comment-share-panl");
							if (_.length <= 0) {
								$(document.body).unbind("click", B);
								A.find(".comment-share-panl").css("display", "none");
							}
						},
						E = function () {
							var _ = A.find(".comment-share-panl");
							if (_.css("display") == "none") {
								_.show();
								$(document.body).bind("click", B);
							} else {
								_.hide();
								$(document.body).unbind("click", B);
							}
						};
						if (FileUtils._mCommentPanel == null) {
							var D = {
								loginState : FileUtils.loginState,
								source_uk : FileUtils.share_uk,
								source_id : FileUtils.share_id,
								type : 0,
								avatar_url : FileUtils.self_avatar_url,
								uname : FileUtils.selfUserName,
								current_uk : FileUtils.self_uk,
								domType : 2,
								limit : 20,
								dialog : {
									hasBorder : true,
									hasArraw : true,
									hasScroll : true,
									left : 311 - (_.width() / 2 + 10)
								}
							};
							C._installCommentPlugin(function () {
								FileUtils._mCommentPanel = new disk.ui.Comment();
								A.append("<div class='comment-share-panl'></div>");
								FileUtils._mCommentPanel.startInit(A.find(".comment-share-panl"), D);
								FileUtils._mCommentPanel.getTotalCount = function (A) {
									var B = A > 999 ? "999+" : A;
									_.text("\u8bc4\u8bba" + (B <= 0 ? "" : "(" + B + ")"));
								};
								E();
							});
						} else {
							E();
						}
					});
				}
			}
			this._startTestPCSCDN();
			if (FileUtils.loginState == 1 && FileUtils.isInWhiteList == 1 && Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				this._installInWhiteList();
			}
			if (Page.inViewMode(Page.VIEW_PROPERTY_OTHER)) {
				$.get(disk.api.RestAPI.SHARE_COUNTER, {
					type : 2,
					shareid : FileUtils.share_id,
					uk : FileUtils.share_uk,
					sign : FileUtils.share_sign,
					timestamp : FileUtils.share_timestamp,
					t : new Date().getTime(),
					_ : Math.random()
				});
			}
			if (Page.inViewMode(Page.VIEW_HOME)) {
				try {
					setTimeout(function () {
						if ($("#panHelperMenu").length > 0 && FileUtils && FileUtils._mDiskLog) {
							FileUtils._mDiskLog.send({
								type : "clouddisk_helper_analytics"
							});
						}
					}, 5000);
				} catch (B) {}

			}
		},
		_startTestPCSCDN : function () {
			ark.use({
				url : "disk.util.pcs_util",
				noDependency : true
			}, function () {
				disk.util.PCSUtils.testPCSCDNConnectivity(function (C, A, _, B, D) {
					if (C == 1) {
						Page.setCookie("cflag", B + ":" + D, 10, "/", ".baidu.com");
						if (disk.DEBUG) {
							console.log("pcs cflag resolved, ", B);
						}
					}
				});
			});
		},
		sendListViewReady : function () {
			var _ = A.DRAG_SELECTION_LISTVIEW_READY;
			if ((this._mViewFlags & _) != _) {
				this._mViewFlags |= _;
				this.installDragSelection();
			}
			this.sendDragSurvallanceConfChanged();
		},
		sendRequirementReady : function () {
			var _ = A.DRAG_SELECTION_REQUIREMENT_READY;
			if ((this._mViewFlags & _) != _) {
				this._mViewFlags |= _;
				this.installDragSelection();
			}
		},
		sendDragSurvallanceConfChanged : function () {
			if (this._mDragSurvallance) {
				this._mDragSurvallance.dispatchConfigChanged();
			}
		},
		installDragSelection : function () {
			var B = A.DRAG_SELECTION_LISTVIEW_READY,
			_ = A.DRAG_SELECTION_REQUIREMENT_READY;
			if ((this._mViewFlags & B) == B && (this._mViewFlags & _) == _) {
				this._installDragSelection();
				if (Page.inViewMode(Page.VIEW_HOME)) {
					if (disk.getParam("frm") == "sharebt") {
						if (FileUtils._mOfflineList == null) {
							FileUtils._mOfflineList = disk.ui.OfflineListDialog.obtain();
						}
						FileUtils._mOfflineList.setVisible(true);
					} else {
						if ((!FileUtils.is_vip) || A.FLAG_SHOW_VIP_DIALOG) {
							disk.Context.requestFeature(disk.Context.FEATURE_STORAGE, {
								succ : function () {
									Page.obtain().installAwardIntrDialog();
								}
							});
						}
					}
				}
			}
		},
		_installDragSelection : function () {
			var _ = new disk.ui.DragSelectionSurveillance({
					target : document.getElementById("fileList")
				}, {
					not : ".input-cbx, .input-cbx s, .input-cbx dfn, .on *"
				}),
			B = this;
			_.install();
			this._mDragSurvallance = _;
			FileUtils._mViewModeSwitchWatcher = function (C) {
				B.log(A.TAG, "switching to gridview: " + C);
				if (C === true) {
					if (!_.getElement(A.TAG_GRIDVIEW) && FileUtils._mInfiniteGridView) {
						B._addDDGrid(_);
					}
					if (_.getElement(A.TAG_GRIDVIEW)) {
						_.switchTo(A.TAG_GRIDVIEW);
					}
				} else {
					if (!_.getElement(A.TAG_LISTVIEW) && FileUtils._mInfiniteListView) {
						B._addDDList(_);
					}
					if (_.getElement(A.TAG_LISTVIEW)) {
						_.switchTo(A.TAG_LISTVIEW);
					}
				}
			};
			FileUtils._mViewModeSwitchWatcher.call(FileUtils, FileUtils.getViewMode() != FileUtils.VIEW_LIST);
		},
		_ddSelectionStart : function () {
			FileUtils._hideRightMouseMenu();
			if (Page.inViewMode(Page.VIEW_SHARE_PROPERTY_OWN) || Page.inViewMode(Page.VIEW_HOME_PAGE_SLAVE)) {
				FileUtils._shareDropdownDrawerTrigger(null, true, 200);
			}
		},
		_addDDGrid : function (_) {
			var B = new disk.ui.GridViewDragSelection(FileUtils._mInfiniteGridView),
			C = this;
			B.onSelectionEnd = function (A, _) {
				Page.obtain().resolveToolbarMenu();
			};
			B.onSelectionStart = function () {
				C._ddSelectionStart();
			};
			B.onComputeThumbWidth = function () {
				this.setThumbWidthResolved();
				var _ = $(".icon-item", "#fileList"),
				A = _.width() + 3 * 2 + 5 * 2;
				return A;
			};
			B.install();
			_.addSurveillance(A.TAG_GRIDVIEW, B);
		},
		_addDDList : function (_) {
			var B = new disk.ui.ListViewDragSelection(FileUtils._mInfiniteListView),
			C = this;
			B.onSelectionStart = function () {
				C._ddSelectionStart();
			};
			B.onSelectionEnd = function (B, _) {
				C.log(A.TAG, "onSelectionEnd: " + B + ", " + _);
				Page.obtain().resolveToolbarMenu();
			};
			B.install();
			_.addSurveillance(A.TAG_LISTVIEW, B);
		},
		onFeatureRequest : function (E, C, B, D) {
			this.setBooleanFlag(_.st.AbstractPageWindow.CLASS_LOADER_OVERRIDE, true);
			this.log(A.TAG, "feature request " + E + ", " + C);
			$.getScript(C).done(B).fail(D);
		},
		onFeatureReady : function (B, _) {
			this.log(A.TAG, "feature ready " + B + "==>" + _);
			switch (B) {
			case A.FEATURE_LETENCY:
				if (_ === true) {
					this._onPageViewLetencyStart();
				} else {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u90e8\u5206\u7ec4\u4ef6\u52a0\u8f7d\u5931\u8d25\uff0c\u8bf7\u5237\u65b0\u9875\u9762\u91cd\u8bd5",
						sticky : false
					});
				}
				break;
			case A.FEATURE_MUSIC_PLAYER:
				if (_ === true) {
					Page.obtain().playMusicAll();
				} else {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u52a0\u8f7d\u97f3\u4e50\u64ad\u653e\u7ec4\u4ef6\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
						sticky : false
					});
				}
				break;
			case A.FEATURE_MUSIC_PLAYER_2:
				if (_ !== true) {
					Utilities.useToast({
						toastMode : disk.ui.Toast.MODE_CAUTION,
						msg : "\u52a0\u8f7d\u97f3\u4e50\u64ad\u653e\u7ec4\u4ef6\u5931\u8d25\uff0c\u8bf7\u7a0d\u5019\u91cd\u8bd5",
						sticky : false
					});
				}
				break;
			default:
				break;
			}
		},
		onFeatureLoading : function (B, _) {
			this.log(A.TAG, "feature loading " + B + "=>" + _);
			switch (B) {
			case A.FEATURE_MUSIC_PLAYER:
			case A.FEATURE_MUSIC_PLAYER_2:
				break;
			default:
				return;
			}
			if (_ !== true) {
				disk.Context.getService(disk.Context.SERVICE_TOAST).setVisible(false);
			} else {
				Utilities.useToast({
					toastMode : disk.ui.Toast.MODE_LOADING,
					msg : "\u6b63\u5728\u52a0\u8f7d\u7ec4\u4ef6\uff0c\u8bf7\u7a0d\u5019&hellip;",
					sticky : true
				});
			}
		},
		onSize : function () {
			this.log(A.TAG, "on lazy sizing");
			if (this._mFM && this._mFM.isVisible()) {
				this._mFM.onResize();
			}
			if (disk.ui.ZoomTips) {
				new disk.ui.ZoomTips();
			} else {
				ark.use({
					url : "disk.util.zoomTips",
					noDependency : true
				}, function () {
					new disk.ui.ZoomTips();
				});
			}
		},
		onSendLog : function (B) {
			this.setBooleanFlag(_.st.AbstractPageWindow.INSTALL_LOGGING_POSTER, true);
			this.log(A.TAG, B);
		}
	});
	_.app.HomePageWindow = A;
})(unify);
(function () {
	function _(_) {
		if (_) {
			this.url = "" || _.url;
			this.logObjectName = "fe_log_" || _.logObjectName;
			this.baseParam = null || _.baseParam;
			this.sendBaseParamTag = _.sendBaseParamTag || true;
		} else {
			this.url = "";
			this.logObjectName = "fe_log_";
			this.baseParam = null;
			this.sendBaseParamTag = true;
		}
		this.UrlLength = 900;
	}
	_.prototype = {
		send : function (E) {
			var H,
			G = E.url || this.url,
			D = new Date().getTime(),
			A = {};
			if (this.sendBaseParamTag) {
				for (H in this.baseParam) {
					A[H] = (typeof this.baseParam[H] == "function") ? this.baseParam[H]() : this.baseParam[H];
				}
			}
			var F = {};
			for (H in E) {
				if (H != "url") {
					F[H] = (typeof E == "function") ? E[H]() : E[H];
				}
			}
			var C = this._getRequestUrl(D, G, A, F);
			for (var B = 0, _ = C.length; B < _; B++) {
				this._request(D, B, C[B]);
			}
		},
		_request : function (B, _, C) {
			var D = this.logObjectName + "_" + B + "_" + _;
			window[D] = new Image();
			var A = window[D];
			A.onload = (A.onerror = function () {
				window[D] = null;
			});
			A.src = C;
			A = null;
		},
		_getRequestUrl : function (C, L, F, M) {
			var D,
			J = [],
			E = [];
			if (F) {
				for (D in F) {
					E.push(D + "=" + F[D]);
				}
			}
			E = E.join("&");
			E = (E == "") ? "" : "&" + E;
			var B = L + "?_lsid=" + C + "&_lsix=1" + E,
			H = 1;
			for (D in M) {
				if (Object.prototype.toString.apply(M[D]) === "[object Array]") {
					var G = M[D],
					I = [],
					A = B.length + D.length + 2;
					for (var K = 0, _ = G.length; K < _; K++) {
						A += G[K].length + 1;
						if (A > this.UrlLength) {
							J.push(B + "&" + D + "=" + I.join());
							H++;
							I = [];
							A = B.length + D.length + 2;
							B = L + "?_lsid=" + C + "&_lsix=" + H + E;
							K--;
						} else {
							I.push(G[K]);
						}
					}
					if (I.length > 0) {
						B += "&" + D + "=" + I.join();
					}
				} else {
					if (B.length + D.length > this.UrlLength) {
						J.push(B);
						H++;
						B = L + "?_lsid=" + C + "&_lsix=" + H + E;
					}
					B += "&" + D + "=" + M[D];
				}
			}
			J.push(B);
			return J;
		}
	};
	disk.FELog = _;
})();
(function () {
	disk.util.X = {};
	_supportPostMessage = (function () {
		if (window.postMessage) {
			try {
				if (window.postMessage.toString().indexOf("[native code]") >= 0) {
					return true;
				} else {
					alert("\u6d4f\u89c8\u5668\u539f\u751f\u7684postMessage\u65b9\u6cd5\u4f3c\u4e4e\u5df2\u88ab\u8986\u76d6\uff0c\u8bf7\u4e0d\u8981\u8986\u76d6\u8be5\u539f\u751f\u65b9\u6cd5\uff0c\u5426\u5219\u8de8\u57df\u901a\u4fe1\u53ef\u80fd\u65e0\u6cd5\u6b63\u5e38\u5de5\u4f5c\uff01");
				}
			} catch (_) {
				return true;
			}
		}
		return false;
	})();
	function C(A) {
		if (A == "*") {
			return "*";
		}
		var _ = /^https?\:\/\/[^\:\/]+(?:\:\d+)?/.exec(decodeURIComponent(A) || location.href);
		return _ ? _[0] : "*";
	}
	var F = disk.util.X.xSender = function (A, _, B) {
		this._target = A;
		this._href = _;
		this._tag = B;
		this._origin = C(this._href);
		this.sameDomain = false;
		try {
			if (this._target.document && document.domain == this._target.document.domain) {
				this.sameDomain = true;
			}
		} catch (D) {}

		if (!this.sameDomain && !_supportPostMessage) {
			this._pool = [];
			this._status = false;
			this._sendLoop();
		}
	};
	F.prototype.send = function (_) {
		_._tag = this._tag;
		if (this.sameDomain) {
			this._target.disk && this._target.disk.util.X.xReceive._dispatch(_, C());
		} else {
			if (_supportPostMessage) {
				this._target.postMessage($.stringify(_), this._origin);
			} else {
				this._pool.push(_);
				if (!this._status) {
					this._status = true;
					this._sendLoop();
				}
			}
		}
	};
	F.prototype._sendLoop = function () {
		if (this._pool.length) {
			this._sendProxy(this._pool.shift());
		} else {
			if (!this._pool.length) {
				this._status = false;
				return;
			}
		}
		var _ = this;
		setTimeout(function () {
			_._sendLoop();
		}, 30);
	};
	F.prototype._sendProxy = function (_) {
		_._timeStamp = Math.random();
		_._origin = this._origin;
		this._target.name = $.stringify(_);
	};
	var B = false,
	D = "";
	function A(A) {
		if (B) {
			return;
		}
		A = A || window;
		if (_supportPostMessage) {
			cfn = function (A) {
				_._dispatchStr(A.data, A.origin);
			};
			if (A.addEventListener) {
				A.addEventListener("message", cfn, false);
			} else {
				if (A.attachEvent) {
					A.attachEvent("onmessage", cfn);
				}
			}
		} else {
			var C = {};
			setInterval(function () {
				if (A.name == D) {
					return;
				}
				D = A.name;
				_._dispatchStr(D);
			}, 20);
		}
		B = true;
	}
	var E = {},
	_ = disk.util.X.xReceive = function (B, _, D) {
		D = D || {};
		!this.sameDomain && A(D.target);
		_ = C(_ || "*");
		if (!E[_]) {
			E[_] = new disk.util.X.Observer();
		}
		E[_].subscribe(function (_) {
			if (D.srcTag && _.data && _.data._tag != D.srcTag) {
				return;
			}
			B(_);
		});
	};
	_._dispatchStr = function (D, A) {
		var B;
		try {
			B = JSON.parse(D);
		} catch (C) {
			return;
		}
		_._dispatch(B, A || B._origin);
	};
	_._dispatch = function (A, _) {
		if (_ && E[_]) {
			E[_].notify({
				data : A,
				origin : _
			});
		}
	};
})();
(function () {
	var _ = disk.util.X.Observer = function (_) {
		this._sender = _;
		this._subscriber = [];
	};
	_.prototype.subscribe = function (E, B, D) {
		if (typeof E !== "function") {
			return false;
		}
		var C = this._subscriber;
		for (var A = 0, _; _ = C[A]; A++) {
			if (_.fn == E) {
				return false;
			}
		}
		C.push({
			fn : E,
			self : B,
			context : D
		});
		return true;
	};
	_.prototype.unsubscribe = function (D) {
		if (typeof D !== "function") {
			return false;
		}
		var B = this._subscriber,
		C = false;
		for (var A = 0, _; _ = B[A]; A++) {
			if (_.fn == D) {
				C = true;
				B = B.slice(0, A).concat(B.slice(A + 1));
				break;
			}
		}
		if (!C) {
			return false;
		}
		this._subscriber = B;
		return true;
	};
	_.prototype.notify = function (C) {
		var B = this._subscriber;
		for (var A = 0, _; _ = B[A]; A++) {
			_.fn.call(_.self, C, this._sender, _.context);
		}
	};
})();
(function () {
	disk.util.X.xReceive(function (B) {
		var A;
		if (!B || !(B = B.data) || !(A = B.key)) {
			return;
		}
		var _ = B.args || {};
		switch (A) {
		default:
			if (typeof disk.util.X[A] == "function") {
				disk.util.X[A].call(disk.util.X, _);
			} else {
				alert("Invalid methd: " + A);
			}
			break;
		}
	}, "*", {
		type : "download_package"
	});
	disk.util.X.pcsPackageFailure = function (A) {
		var _ = disk.Context.getService(disk.Context.SERVICE_DIALOG);
		_.setVisible(true);
		_.setMessage("\u8bf7\u5206\u6279\u4e0b\u8f7d", "\u60a8\u4e0b\u8f7d\u7684\u6587\u4ef6\u8d85\u8fc7\u4e86400M\uff0c\u8bf7\u5206\u6279\u9009\u62e9\u4e0b\u8f7d\uff01", function () {
			this.setVisible(false);
		});
	};
})();
(function (E) {
	var C = 0,
	F = function () {},
	D = function (_) {
		return typeof _ === "function" && (_.prototype && _ === _.prototype.constructor);
	},
	B = function () {
		var A = _._calledMethodHost.prototype[_._calledMethod];
		return A ? A.apply(_._calledThis, _._calledArguments) : undefined;
	};
	function _(E, A) {
		if (!A) {
			A = E;
			E = Object;
		}
		A = (typeof A === "function" ? A() : A) || {};
		A.init = A.hasOwnProperty("init") ? A.init : F;
		var D = function () {
			this.init.apply(this, arguments);
		};
		D.NAME = A.NAME || _.getId();
		D.HTML = A.HTML || "";
		D.parent = E;
		var C = function () {};
		C.prototype = E.prototype;
		D.prototype = new C();
		D.prototype.constructor = D.prototype.c = D;
		D.prototype.toString = function () {
			return A.name || "";
		};
		D.prototype.up = B;
		_.hookPrototypeMethod(D, A);
		return D;
	}
	_.getId = function () {
		return C++;
	};
	_._ignoreHook = {};
	var A = function (A, B, C) {
		return function () {
			_._calledThis = this;
			_._calledMethod = B;
			_._calledArguments = arguments;
			_._calledMethodHost = A.parent;
			return C.apply(this, arguments);
		};
	};
	_.hookPrototypeMethod = function (D, B) {
		for (var C in B) {
			if (B.hasOwnProperty(C) && typeof B[C] === "function" && !_._ignoreHook[C]) {
				D.prototype[C] = A(D, C, B[C]);
			}
		}
	};
	E.isClass = D;
	E.Class = _;
})(window);
