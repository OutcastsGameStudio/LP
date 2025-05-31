param(
    [string]$InputFile = "..\Content\Characters\ANGENE\Mesh\ANGENE.uasset.gz",
    [string]$OutputFile = "",
    [switch]$KeepArchive = $false
)

Add-Type -AssemblyName System.IO.Compression

try {
    $FullInputPath = Resolve-Path $InputFile -ErrorAction Stop
    $InputFile = $FullInputPath.Path
    
    if ([string]::IsNullOrEmpty($OutputFile)) {
        $OutputFile = $InputFile -replace '\.gz$', ''
    }
    
    # Créer le répertoire de sortie si nécessaire
    $OutputDir = Split-Path $OutputFile -Parent
    if (![string]::IsNullOrEmpty($OutputDir) -and !(Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    }
    
    Write-Host "Decompression en cours..." -ForegroundColor Green
    Write-Host "Archive: $([System.IO.Path]::GetFileName($InputFile))" -ForegroundColor Gray
    Write-Host "Destination: $OutputFile" -ForegroundColor Gray
    
    $inputFileStream = $null
    $outputFileStream = $null
    $gzipStream = $null
    
    try {
        $inputFileStream = [System.IO.File]::OpenRead($InputFile)
        $outputFileStream = [System.IO.File]::Create($OutputFile)
        
        # Creer le flux de decompression
        $gzipStream = [System.IO.Compression.GZipStream]::new(
            $inputFileStream, 
            [System.IO.Compression.CompressionMode]::Decompress
        )
        
        # Copier les donnees decompressees
        $gzipStream.CopyTo($outputFileStream)
        
    } finally {
        if ($gzipStream) { $gzipStream.Close(); $gzipStream.Dispose() }
        if ($outputFileStream) { $outputFileStream.Close(); $outputFileStream.Dispose() }
        if ($inputFileStream) { $inputFileStream.Close(); $inputFileStream.Dispose() }
    }
    
    # Vérifier que la décompression s'est bien passée
    if (!(Test-Path $OutputFile)) {
        throw "Le fichier decompresse n'a pas ete cree correctement"
    }
    
    # Calculer les tailles
    $archiveSize = (Get-Item $InputFile).Length
    $decompressedSize = (Get-Item $OutputFile).Length
    $expansionRatio = [math]::Round(($decompressedSize / $archiveSize), 2)
    
    Write-Host "Decompression terminee!" -ForegroundColor Green
    Write-Host "Archive: $([math]::Round($archiveSize / 1MB, 2)) MB" -ForegroundColor Yellow
    Write-Host "Fichier decompresse: $([math]::Round($decompressedSize / 1MB, 2)) MB" -ForegroundColor Yellow
    Write-Host "Ratio d'expansion: ${expansionRatio}x" -ForegroundColor Cyan
    
    # Supprimer l'archive (sauf si -KeepArchive est spécifié)
    if (!$KeepArchive) {
        Write-Host "Suppression de l'archive..." -ForegroundColor Yellow
        Remove-Item $InputFile -Force
        Write-Host "Archive supprimee: $(Split-Path $InputFile -Leaf)" -ForegroundColor Green
    } else {
        Write-Host "Archive conservee" -ForegroundColor Cyan
    }
    
} catch [System.Management.Automation.ItemNotFoundException] {
    Write-Error "Le fichier '$InputFile' n'existe pas!"
    Write-Host "Repertoire actuel: $(Get-Location)" -ForegroundColor Yellow
    exit 1
} catch {
    Write-Error "Erreur lors de la decompression: $($_.Exception.Message)"
    Write-Host "Type d'erreur: $($_.Exception.GetType().Name)" -ForegroundColor Red
    
    # Si la décompression a échoué, supprimer le fichier de sortie partiel s'il existe
    if (Test-Path $OutputFile) {
        try {
            Remove-Item $OutputFile -Force
            Write-Host "Fichier de sortie partiel supprime" -ForegroundColor Yellow
        } catch {
            Write-Warning "Impossible de supprimer le fichier de sortie partiel: $OutputFile"
        }
    }
    exit 1
}