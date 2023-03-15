export namespace slim {
	export namespace types {
		export interface KeyValueAny {
			[key:string]: unknown;
		}
        export type iPair<K, V> = {
            [index in K]: V;
        }
	}
}