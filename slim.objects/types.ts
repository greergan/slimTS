export interface KeyValueAny {
	[key:string]:any
}
export interface KeyValueString {
	[key:string]:string
}
export interface KeyValueStringArray {
	[key:string]:Array<string>
}
/*
export type LinkedList<Type> = Type & { next: LinkedList<Type> };
*/