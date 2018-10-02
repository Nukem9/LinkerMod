function AddVersions(releases) {
	const versionList = document.getElementById("versions");
	for (const release of releases) {
		const elem = document.createElement("a");
		elem.innerText = release.name;
		elem.href = release.url;
		versionList.appendChild(elem);
		versionList.appendChild(document.createElement("br"));
	}
}

window.onload = () => {
	const api_url = "https://api.github.com/repos/Nukem9/LinkerMod/releases";
	fetch(api_url).then((value) => {
		return value.json();
	}).then((result) => {
		AddVersions(result);
		const elem = document.getElementById("debug");
		elem.innerText = "";
		for (const release of result) {
			elem.innerText += JSON.stringify(release) + "\n";
		}
		// document.getElementById("output").innerText = result;
	});
};