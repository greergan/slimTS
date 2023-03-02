import { debug, error, todo, trace, SlimTypes} from './mod.ts';
import { merge, get_file, normalize_url } from './mod.ts';
export class SlimView {
	private name_space_url:string;
	private raw_views:Map<string, string> = new Map<string, string>();
	private compiled_views:Map<string, string> = new Map<string, string>();
	private with_view_dependencies:Map<string, Array<string>> = new Map<string, Array<string>>();
	constructor(url?:string) {
		this.name_space_url = (url) ? url : "";
		this.name_space_url.trim();
	}
	async compile(url:string): Promise<string> {
		let normalized_url:string = normalize_url(url);
		if(this.compiled_views.has(normalized_url)) {
			return this.compiled_views.get(normalized_url) || "";
		}
		if(!this.raw_views.has(normalized_url)) {
			this.raw_views.set(normalized_url, await get_file(normalized_url));
		}
		let view_string = this.raw_views.get(normalized_url)!;
		const include_regex =/<\s*include\s+view\s*=\s*"\s*([a-z0-9-_/?&=]+?)\s*"\s*\/?\s*>/gmi;

		const fetch_file = (file:string) => Promise.resolve(this.compile(file));
		const promises:Array<Promise<string>> = [];
		view_string = view_string.replace(include_regex, ($0:string, url:string):string => {
			let url_string = normalize_url((url.match(/^https:\/\/|http:\/\/|file:\/\/\//i)) ? url : this.name_space_url + url);
			promises.push(fetch_file(url_string));
			return $0;
		});
		await Promise.all(promises).then((results) => {
			view_string = view_string.replace(include_regex, () => results.shift() || "");
		});
		this.compiled_views.set(url, view_string);
		return view_string;
	}
	public recompile(url:string) {
		let normalized_url:string = (url) ? normalize_url(url) : this.name_space_url;
		if(this.raw_views.has(normalized_url)) {
			this.raw_views.delete(normalized_url);
		}
		if(this.compiled_views.has(normalized_url)) {
			this.compiled_views.delete(normalized_url);
		}
		this.compile(normalized_url);
		if(this.with_view_dependencies.has(normalized_url)) {
			const dependencies = this.with_view_dependencies.get(normalized_url) || [];
			for(const dependency in dependencies) {
				this.recompile(dependency);
			}
		}
	}
	public async render(model:object, url:string): Promise<string> {
		const compiled_view:string = await this.compile(url);
		return await merge(model, compiled_view, this);
	}
	private add_dependent_view(with_view:string, view:string) {
		let dependant_views:Array<string> = this.with_view_dependencies.get(with_view) || [];
		if(!dependant_views.indexOf(view)) {
			dependant_views.push(view);
		}
		this.with_view_dependencies.set(with_view, dependant_views);
	}
}