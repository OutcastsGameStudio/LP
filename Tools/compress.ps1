# Script de compression pour fichiers .uasset
param(
    [string]$InputFile = "..\Content\Characters\ANGENE\Mesh\ANGENE.uasset",
    [string]$OutputFile = "",
    [string]$CompressionLevel = "Optimal",
    [switch]$KeepOriginal = $false
)

# Ajouter les assemblys nécessaires
Add-Type -AssemblyName System.IO.Compression

try {
    # Résoudre le chemin complet du fichier d'entrée
    $FullInputPath = Resolve-Path $InputFile -ErrorAction Stop
    $InputFile = $FullInputPath.Path
    
    # Définir le fichier de sortie avec chemin absolu
    if ([string]::IsNullOrEmpty($OutputFile)) {
        $OutputFile = $InputFile + ".gz"
    } else {
        if (![System.IO.Path]::IsPathRooted($OutputFile)) {
            $OutputFile = Join-Path (Get-Location) $OutputFile
        }
    }
    
    # Créer le répertoire de sortie si nécessaire
    $OutputDir = Split-Path $OutputFile -Parent
    if (!(Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    }
    
    Write-Host "Compression en cours..." -ForegroundColor Green
    Write-Host "Source: $InputFile" -ForegroundColor Gray
    Write-Host "Destination: $OutputFile" -ForegroundColor Gray
    
    # Lire le fichier source
    $inputBytes = [System.IO.File]::ReadAllBytes($InputFile)
    
    # Créer le flux de sortie
    $outputStream = [System.IO.File]::Create($OutputFile)
    
    # Créer le flux de compression GZip avec constructeur explicite
    $compressionMode = [System.IO.Compression.CompressionMode]::Compress
    $compressionLevel = [System.IO.Compression.CompressionLevel]::$CompressionLevel
    $gzipStream = New-Object System.IO.Compression.GZipStream($outputStream, $compressionMode)
    
    # Écrire les données compressées
    $gzipStream.Write($inputBytes, 0, $inputBytes.Length)
    
    # Fermer les flux dans le bon ordre
    $gzipStream.Flush()
    $gzipStream.Close()
    $gzipStream.Dispose()
    $outputStream.Close()
    $outputStream.Dispose()
    
    # Vérifier que la compression s'est bien passée
    if (!(Test-Path $OutputFile)) {
        throw "Le fichier compressé n'a pas été créé correctement"
    }
    
    # Calculer les tailles
    $originalSize = (Get-Item $InputFile).Length
    $compressedSize = (Get-Item $OutputFile).Length
    $compressionRatio = [math]::Round((1 - ($compressedSize / $originalSize)) * 100, 2)
    
    Write-Host "Compression terminée!" -ForegroundColor Green
    Write-Host "Fichier original: $([math]::Round($originalSize / 1MB, 2)) MB" -ForegroundColor Yellow
    Write-Host "Fichier compressé: $([math]::Round($compressedSize / 1MB, 2)) MB" -ForegroundColor Yellow
    Write-Host "Taux de compression: $compressionRatio%" -ForegroundColor Cyan
    
    # Supprimer le fichier original (sauf si -KeepOriginal est spécifié)
    if (!$KeepOriginal) {
        Write-Host "Suppression du fichier original..." -ForegroundColor Yellow
        Remove-Item $InputFile -Force
        Write-Host "Fichier original supprimé: $(Split-Path $InputFile -Leaf)" -ForegroundColor Green
    } else {
        Write-Host "Fichier original conservé" -ForegroundColor Cyan
    }
    
} catch [System.Management.Automation.ItemNotFoundException] {
    Write-Error "Le fichier '$InputFile' n'existe pas!"
    Write-Host "Répertoire actuel: $(Get-Location)" -ForegroundColor Yellow
    exit 1
} catch {
    Write-Error "Erreur lors de la compression: $($_.Exception.Message)"
    Write-Host "Type d'erreur: $($_.Exception.GetType().Name)" -ForegroundColor Red
    
    # Si la compression a échoué, supprimer le fichier de sortie partiel s'il existe
    if (Test-Path $OutputFile) {
        try {
            Remove-Item $OutputFile -Force
            Write-Host "Fichier de sortie partiel supprimé" -ForegroundColor Yellow
        } catch {
            Write-Warning "Impossible de supprimer le fichier de sortie partiel: $OutputFile"
        }
    }
    exit 1
}